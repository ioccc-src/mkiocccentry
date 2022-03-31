/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jint - convert JSON integer strings
 *
 * "Because the JSON co-founders flawed minimalism is sub-minimal." :-)
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

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>

/*
 * JSON - JSON structures and functions
 */
#include "json.h"


/* DO NOT REMOVE THIS LINE - jint tests are added below this line */
#if defined(JFLOAT_TEST_ENABLED)
#define TEST_COUNT (285)

int const test_count = TEST_COUNT;

char *test_set[TEST_COUNT+1] = {
    "-8589934594",
    "-8589934594.0",
    "-8589934594.0e0",
    "-8589934593",
    "-8589934593.0",
    "-8589934593.0e0",
    "-8589934592",
    "-8589934592.0",
    "-8589934592.0e0",
    "-8589934591",
    "-8589934591.0",
    "-8589934591.0e0",
    "-8589934590",
    "-8589934590.0",
    "-8589934590.0e0",
    "-4294967298",
    "-4294967298.0",
    "-4294967298.0e0",
    "-4294967297",
    "-4294967297.0",
    "-4294967297.0e0",
    "-4294967296",
    "-4294967296.0",
    "-4294967296.0e0",
    "-4294967295",
    "-4294967295.0",
    "-4294967295.0e0",
    "-4294967294",
    "-4294967294.0",
    "-4294967294.0e0",
    "-2147483650",
    "-2147483650.0",
    "-2147483650.0e0",
    "-2147483649",
    "-2147483649.0",
    "-2147483649.0e0",
    "-2147483648",
    "-2147483648.0",
    "-2147483648.0e0",
    "-2147483647",
    "-2147483647.0",
    "-2147483647.0e0",
    "-2147483646",
    "-2147483646.0",
    "-2147483646.0e0",
    "-131074",
    "-131074.0",
    "-131074.0e0",
    "-131073",
    "-131073.0",
    "-131073.0e0",
    "-131072",
    "-131072.0",
    "-131072.0e0",
    "-131071",
    "-131071.0",
    "-131071.0e0",
    "-131070",
    "-131070.0",
    "-131070.0e0",
    "-65538",
    "-65538.0",
    "-65538.0e0",
    "-65537",
    "-65537.0",
    "-65537.0e0",
    "-65536",
    "-65536.0",
    "-65536.0e0",
    "-65535",
    "-65535.0",
    "-65535.0e0",
    "-65534",
    "-65534.0",
    "-65534.0e0",
    "-32770",
    "-32770.0",
    "-32770.0e0",
    "-32769",
    "-32769.0",
    "-32769.0e0",
    "-32768",
    "-32768.0",
    "-32768.0e0",
    "-32767",
    "-32767.0",
    "-32767.0e0",
    "-32766",
    "-32766.0",
    "-32766.0e0",
    "-514",
    "-514.0",
    "-514.0e0",
    "-513",
    "-513.0",
    "-513.0e0",
    "-512",
    "-512.0",
    "-512.0e0",
    "-511",
    "-511.0",
    "-511.0e0",
    "-510",
    "-510.0",
    "-510.0e0",
    "-258",
    "-258.0",
    "-258.0e0",
    "-257",
    "-257.0",
    "-257.0e0",
    "-256",
    "-256.0",
    "-256.0e0",
    "-255",
    "-255.0",
    "-255.0e0",
    "-254",
    "-254.0",
    "-254.0e0",
    "-130",
    "-130.0",
    "-130.0e0",
    "-129",
    "-129.0",
    "-129.0e0",
    "-128",
    "-128.0",
    "-128.0e0",
    "-127",
    "-127.0",
    "-127.0e0",
    "-126",
    "-126.0",
    "-126.0e0",
    "-2",
    "-2.0",
    "-2.0e0",
    "-1",
    "-1.0",
    "-1.0e0",
    "0",
    "0.0",
    "0.0e0",
    "1",
    "1.0",
    "1.0e0",
    "2",
    "2.0",
    "2.0e0",
    "126",
    "126.0",
    "126.0e0",
    "127",
    "127.0",
    "127.0e0",
    "128",
    "128.0",
    "128.0e0",
    "129",
    "129.0",
    "129.0e0",
    "130",
    "130.0",
    "130.0e0",
    "254",
    "254.0",
    "254.0e0",
    "255",
    "255.0",
    "255.0e0",
    "256",
    "256.0",
    "256.0e0",
    "257",
    "257.0",
    "257.0e0",
    "258",
    "258.0",
    "258.0e0",
    "510",
    "510.0",
    "510.0e0",
    "511",
    "511.0",
    "511.0e0",
    "512",
    "512.0",
    "512.0e0",
    "513",
    "513.0",
    "513.0e0",
    "514",
    "514.0",
    "514.0e0",
    "32766",
    "32766.0",
    "32766.0e0",
    "32767",
    "32767.0",
    "32767.0e0",
    "32768",
    "32768.0",
    "32768.0e0",
    "32769",
    "32769.0",
    "32769.0e0",
    "32770",
    "32770.0",
    "32770.0e0",
    "65534",
    "65534.0",
    "65534.0e0",
    "65535",
    "65535.0",
    "65535.0e0",
    "65536",
    "65536.0",
    "65536.0e0",
    "65537",
    "65537.0",
    "65537.0e0",
    "65538",
    "65538.0",
    "65538.0e0",
    "131070",
    "131070.0",
    "131070.0e0",
    "131071",
    "131071.0",
    "131071.0e0",
    "131072",
    "131072.0",
    "131072.0e0",
    "131073",
    "131073.0",
    "131073.0e0",
    "131074",
    "131074.0",
    "131074.0e0",
    "2147483646",
    "2147483646.0",
    "2147483646.0e0",
    "2147483647",
    "2147483647.0",
    "2147483647.0e0",
    "2147483648",
    "2147483648.0",
    "2147483648.0e0",
    "2147483649",
    "2147483649.0",
    "2147483649.0e0",
    "2147483650",
    "2147483650.0",
    "2147483650.0e0",
    "4294967294",
    "4294967294.0",
    "4294967294.0e0",
    "4294967295",
    "4294967295.0",
    "4294967295.0e0",
    "4294967296",
    "4294967296.0",
    "4294967296.0e0",
    "4294967297",
    "4294967297.0",
    "4294967297.0e0",
    "4294967298",
    "4294967298.0",
    "4294967298.0e0",
    "8589934590",
    "8589934590.0",
    "8589934590.0e0",
    "8589934591",
    "8589934591.0",
    "8589934591.0e0",
    "8589934592",
    "8589934592.0",
    "8589934592.0e0",
    "8589934593",
    "8589934593.0",
    "8589934593.0e0",
    "8589934594",
    "8589934594.0",
    "8589934594.0e0",
    NULL
};

struct floating test_result[TEST_COUNT] = {
    /* test_result[0]: "-8589934594" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934594",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594,	/* JSON floating point value in long double form */
    },

    /* test_result[1]: "-8589934594.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934594.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594,	/* JSON floating point value in long double form */
    },

    /* test_result[2]: "-8589934594.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934594.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594,	/* JSON floating point value in long double form */
    },

    /* test_result[3]: "-8589934593" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934593",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[4]: "-8589934593.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934593.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[5]: "-8589934593.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934593.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[6]: "-8589934592" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934592",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[7]: "-8589934592.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934592.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[8]: "-8589934592.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934592.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[9]: "-8589934591" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934591",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[10]: "-8589934591.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934591.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[11]: "-8589934591.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934591.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[12]: "-8589934590" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934590",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[13]: "-8589934590.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934590.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[14]: "-8589934590.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-8589934590.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[15]: "-4294967298" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967298",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[16]: "-4294967298.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967298.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[17]: "-4294967298.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967298.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[18]: "-4294967297" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967297",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[19]: "-4294967297.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967297.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[20]: "-4294967297.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967297.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[21]: "-4294967296" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967296",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[22]: "-4294967296.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967296.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[23]: "-4294967296.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967296.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[24]: "-4294967295" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967295",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[25]: "-4294967295.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967295.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[26]: "-4294967295.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967295.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[27]: "-4294967294" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967294",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[28]: "-4294967294.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967294.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[29]: "-4294967294.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-4294967294.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[30]: "-2147483650" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483650",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[31]: "-2147483650.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483650.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[32]: "-2147483650.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483650.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[33]: "-2147483649" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483649",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[34]: "-2147483649.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483649.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[35]: "-2147483649.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483649.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[36]: "-2147483648" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483648",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[37]: "-2147483648.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483648.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[38]: "-2147483648.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483648.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[39]: "-2147483647" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483647",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[40]: "-2147483647.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483647.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[41]: "-2147483647.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483647.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[42]: "-2147483646" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483646",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[43]: "-2147483646.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483646.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[44]: "-2147483646.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2147483646.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[45]: "-131074" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131074",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074,	/* JSON floating point value in long double form */
    },

    /* test_result[46]: "-131074.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131074.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074,	/* JSON floating point value in long double form */
    },

    /* test_result[47]: "-131074.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131074.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074,	/* JSON floating point value in long double form */
    },

    /* test_result[48]: "-131073" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131073",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073,	/* JSON floating point value in long double form */
    },

    /* test_result[49]: "-131073.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131073.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073,	/* JSON floating point value in long double form */
    },

    /* test_result[50]: "-131073.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131073.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073,	/* JSON floating point value in long double form */
    },

    /* test_result[51]: "-131072" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131072",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072,	/* JSON floating point value in long double form */
    },

    /* test_result[52]: "-131072.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131072.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072,	/* JSON floating point value in long double form */
    },

    /* test_result[53]: "-131072.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131072.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072,	/* JSON floating point value in long double form */
    },

    /* test_result[54]: "-131071" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131071",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071,	/* JSON floating point value in long double form */
    },

    /* test_result[55]: "-131071.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131071.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071,	/* JSON floating point value in long double form */
    },

    /* test_result[56]: "-131071.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131071.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071,	/* JSON floating point value in long double form */
    },

    /* test_result[57]: "-131070" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131070",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070,	/* JSON floating point value in long double form */
    },

    /* test_result[58]: "-131070.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131070.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070,	/* JSON floating point value in long double form */
    },

    /* test_result[59]: "-131070.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-131070.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070,	/* JSON floating point value in long double form */
    },

    /* test_result[60]: "-65538" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65538",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538,	/* JSON floating point value in long double form */
    },

    /* test_result[61]: "-65538.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65538.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538,	/* JSON floating point value in long double form */
    },

    /* test_result[62]: "-65538.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65538.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538,	/* JSON floating point value in long double form */
    },

    /* test_result[63]: "-65537" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65537",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537,	/* JSON floating point value in long double form */
    },

    /* test_result[64]: "-65537.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65537.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537,	/* JSON floating point value in long double form */
    },

    /* test_result[65]: "-65537.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65537.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537,	/* JSON floating point value in long double form */
    },

    /* test_result[66]: "-65536" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65536",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536,	/* JSON floating point value in long double form */
    },

    /* test_result[67]: "-65536.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65536.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536,	/* JSON floating point value in long double form */
    },

    /* test_result[68]: "-65536.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65536.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536,	/* JSON floating point value in long double form */
    },

    /* test_result[69]: "-65535" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65535",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535,	/* JSON floating point value in long double form */
    },

    /* test_result[70]: "-65535.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65535.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535,	/* JSON floating point value in long double form */
    },

    /* test_result[71]: "-65535.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65535.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535,	/* JSON floating point value in long double form */
    },

    /* test_result[72]: "-65534" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65534",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534,	/* JSON floating point value in long double form */
    },

    /* test_result[73]: "-65534.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65534.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534,	/* JSON floating point value in long double form */
    },

    /* test_result[74]: "-65534.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-65534.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534,	/* JSON floating point value in long double form */
    },

    /* test_result[75]: "-32770" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32770",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770,	/* JSON floating point value in long double form */
    },

    /* test_result[76]: "-32770.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32770.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770,	/* JSON floating point value in long double form */
    },

    /* test_result[77]: "-32770.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32770.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770,	/* JSON floating point value in long double form */
    },

    /* test_result[78]: "-32769" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32769",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769,	/* JSON floating point value in long double form */
    },

    /* test_result[79]: "-32769.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32769.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769,	/* JSON floating point value in long double form */
    },

    /* test_result[80]: "-32769.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32769.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769,	/* JSON floating point value in long double form */
    },

    /* test_result[81]: "-32768" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32768",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768,	/* JSON floating point value in long double form */
    },

    /* test_result[82]: "-32768.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32768.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768,	/* JSON floating point value in long double form */
    },

    /* test_result[83]: "-32768.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32768.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768,	/* JSON floating point value in long double form */
    },

    /* test_result[84]: "-32767" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32767",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767,	/* JSON floating point value in long double form */
    },

    /* test_result[85]: "-32767.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32767.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767,	/* JSON floating point value in long double form */
    },

    /* test_result[86]: "-32767.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32767.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767,	/* JSON floating point value in long double form */
    },

    /* test_result[87]: "-32766" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32766",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766,	/* JSON floating point value in long double form */
    },

    /* test_result[88]: "-32766.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32766.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766,	/* JSON floating point value in long double form */
    },

    /* test_result[89]: "-32766.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-32766.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766,	/* JSON floating point value in long double form */
    },

    /* test_result[90]: "-514" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-514",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-514,	/* JSON floating point value in long double form */
    },

    /* test_result[91]: "-514.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-514.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-514,	/* JSON floating point value in long double form */
    },

    /* test_result[92]: "-514.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-514.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-514,	/* JSON floating point value in long double form */
    },

    /* test_result[93]: "-513" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-513",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-513,	/* JSON floating point value in long double form */
    },

    /* test_result[94]: "-513.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-513.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-513,	/* JSON floating point value in long double form */
    },

    /* test_result[95]: "-513.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-513.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-513,	/* JSON floating point value in long double form */
    },

    /* test_result[96]: "-512" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-512",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-512,	/* JSON floating point value in long double form */
    },

    /* test_result[97]: "-512.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-512.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-512,	/* JSON floating point value in long double form */
    },

    /* test_result[98]: "-512.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-512.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-512,	/* JSON floating point value in long double form */
    },

    /* test_result[99]: "-511" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-511",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-511,	/* JSON floating point value in long double form */
    },

    /* test_result[100]: "-511.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-511.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-511,	/* JSON floating point value in long double form */
    },

    /* test_result[101]: "-511.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-511.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-511,	/* JSON floating point value in long double form */
    },

    /* test_result[102]: "-510" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-510",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-510,	/* JSON floating point value in long double form */
    },

    /* test_result[103]: "-510.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-510.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-510,	/* JSON floating point value in long double form */
    },

    /* test_result[104]: "-510.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-510.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-510,	/* JSON floating point value in long double form */
    },

    /* test_result[105]: "-258" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-258",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-258,	/* JSON floating point value in long double form */
    },

    /* test_result[106]: "-258.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-258.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-258,	/* JSON floating point value in long double form */
    },

    /* test_result[107]: "-258.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-258.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-258,	/* JSON floating point value in long double form */
    },

    /* test_result[108]: "-257" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-257",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-257,	/* JSON floating point value in long double form */
    },

    /* test_result[109]: "-257.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-257.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-257,	/* JSON floating point value in long double form */
    },

    /* test_result[110]: "-257.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-257.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-257,	/* JSON floating point value in long double form */
    },

    /* test_result[111]: "-256" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-256",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-256,	/* JSON floating point value in long double form */
    },

    /* test_result[112]: "-256.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-256.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-256,	/* JSON floating point value in long double form */
    },

    /* test_result[113]: "-256.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-256.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-256,	/* JSON floating point value in long double form */
    },

    /* test_result[114]: "-255" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-255",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-255,	/* JSON floating point value in long double form */
    },

    /* test_result[115]: "-255.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-255.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-255,	/* JSON floating point value in long double form */
    },

    /* test_result[116]: "-255.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-255.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-255,	/* JSON floating point value in long double form */
    },

    /* test_result[117]: "-254" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-254",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-254,	/* JSON floating point value in long double form */
    },

    /* test_result[118]: "-254.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-254.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-254,	/* JSON floating point value in long double form */
    },

    /* test_result[119]: "-254.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-254.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-254,	/* JSON floating point value in long double form */
    },

    /* test_result[120]: "-130" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-130",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-130,	/* JSON floating point value in long double form */
    },

    /* test_result[121]: "-130.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-130.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-130,	/* JSON floating point value in long double form */
    },

    /* test_result[122]: "-130.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-130.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-130,	/* JSON floating point value in long double form */
    },

    /* test_result[123]: "-129" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-129",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-129,	/* JSON floating point value in long double form */
    },

    /* test_result[124]: "-129.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-129.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-129,	/* JSON floating point value in long double form */
    },

    /* test_result[125]: "-129.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-129.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-129,	/* JSON floating point value in long double form */
    },

    /* test_result[126]: "-128" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-128",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-128,	/* JSON floating point value in long double form */
    },

    /* test_result[127]: "-128.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-128.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-128,	/* JSON floating point value in long double form */
    },

    /* test_result[128]: "-128.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-128.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-128,	/* JSON floating point value in long double form */
    },

    /* test_result[129]: "-127" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-127",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-127,	/* JSON floating point value in long double form */
    },

    /* test_result[130]: "-127.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-127.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-127,	/* JSON floating point value in long double form */
    },

    /* test_result[131]: "-127.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-127.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-127,	/* JSON floating point value in long double form */
    },

    /* test_result[132]: "-126" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-126",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-126,	/* JSON floating point value in long double form */
    },

    /* test_result[133]: "-126.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-126.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-126,	/* JSON floating point value in long double form */
    },

    /* test_result[134]: "-126.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-126.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-126,	/* JSON floating point value in long double form */
    },

    /* test_result[135]: "-2" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2,	/* JSON floating point value in long double form */
    },

    /* test_result[136]: "-2.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2,	/* JSON floating point value in long double form */
    },

    /* test_result[137]: "-2.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-2.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-2,	/* JSON floating point value in long double form */
    },

    /* test_result[138]: "-1" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-1",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-1,	/* JSON floating point value in long double form */
    },

    /* test_result[139]: "-1.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-1.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-1,	/* JSON floating point value in long double form */
    },

    /* test_result[140]: "-1.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"-1.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	-1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	-1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	-1,	/* JSON floating point value in long double form */
    },

    /* test_result[141]: "0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	0,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	0,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	0,	/* JSON floating point value in long double form */
    },

    /* test_result[142]: "0.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"0.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	0,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	0,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	0,	/* JSON floating point value in long double form */
    },

    /* test_result[143]: "0.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"0.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	0,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	0,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	0,	/* JSON floating point value in long double form */
    },

    /* test_result[144]: "1" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"1",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	1,	/* JSON floating point value in long double form */
    },

    /* test_result[145]: "1.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"1.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	1,	/* JSON floating point value in long double form */
    },

    /* test_result[146]: "1.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"1.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	1,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	1,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	1,	/* JSON floating point value in long double form */
    },

    /* test_result[147]: "2" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2,	/* JSON floating point value in long double form */
    },

    /* test_result[148]: "2.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2,	/* JSON floating point value in long double form */
    },

    /* test_result[149]: "2.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2,	/* JSON floating point value in long double form */
    },

    /* test_result[150]: "126" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"126",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	126,	/* JSON floating point value in long double form */
    },

    /* test_result[151]: "126.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"126.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	126,	/* JSON floating point value in long double form */
    },

    /* test_result[152]: "126.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"126.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	126,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	126,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	126,	/* JSON floating point value in long double form */
    },

    /* test_result[153]: "127" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"127",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	127,	/* JSON floating point value in long double form */
    },

    /* test_result[154]: "127.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"127.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	127,	/* JSON floating point value in long double form */
    },

    /* test_result[155]: "127.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"127.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	127,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	127,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	127,	/* JSON floating point value in long double form */
    },

    /* test_result[156]: "128" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"128",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	128,	/* JSON floating point value in long double form */
    },

    /* test_result[157]: "128.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"128.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	128,	/* JSON floating point value in long double form */
    },

    /* test_result[158]: "128.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"128.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	128,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	128,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	128,	/* JSON floating point value in long double form */
    },

    /* test_result[159]: "129" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"129",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	129,	/* JSON floating point value in long double form */
    },

    /* test_result[160]: "129.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"129.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	129,	/* JSON floating point value in long double form */
    },

    /* test_result[161]: "129.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"129.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	129,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	129,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	129,	/* JSON floating point value in long double form */
    },

    /* test_result[162]: "130" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"130",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	130,	/* JSON floating point value in long double form */
    },

    /* test_result[163]: "130.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"130.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	130,	/* JSON floating point value in long double form */
    },

    /* test_result[164]: "130.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"130.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	130,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	130,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	130,	/* JSON floating point value in long double form */
    },

    /* test_result[165]: "254" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"254",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	254,	/* JSON floating point value in long double form */
    },

    /* test_result[166]: "254.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"254.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	254,	/* JSON floating point value in long double form */
    },

    /* test_result[167]: "254.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"254.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	254,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	254,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	254,	/* JSON floating point value in long double form */
    },

    /* test_result[168]: "255" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"255",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	255,	/* JSON floating point value in long double form */
    },

    /* test_result[169]: "255.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"255.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	255,	/* JSON floating point value in long double form */
    },

    /* test_result[170]: "255.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"255.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	255,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	255,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	255,	/* JSON floating point value in long double form */
    },

    /* test_result[171]: "256" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"256",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	256,	/* JSON floating point value in long double form */
    },

    /* test_result[172]: "256.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"256.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	256,	/* JSON floating point value in long double form */
    },

    /* test_result[173]: "256.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"256.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	256,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	256,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	256,	/* JSON floating point value in long double form */
    },

    /* test_result[174]: "257" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"257",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	257,	/* JSON floating point value in long double form */
    },

    /* test_result[175]: "257.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"257.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	257,	/* JSON floating point value in long double form */
    },

    /* test_result[176]: "257.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"257.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	257,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	257,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	257,	/* JSON floating point value in long double form */
    },

    /* test_result[177]: "258" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"258",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	258,	/* JSON floating point value in long double form */
    },

    /* test_result[178]: "258.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"258.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	258,	/* JSON floating point value in long double form */
    },

    /* test_result[179]: "258.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"258.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	258,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	258,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	258,	/* JSON floating point value in long double form */
    },

    /* test_result[180]: "510" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"510",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	510,	/* JSON floating point value in long double form */
    },

    /* test_result[181]: "510.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"510.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	510,	/* JSON floating point value in long double form */
    },

    /* test_result[182]: "510.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"510.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	510,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	510,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	510,	/* JSON floating point value in long double form */
    },

    /* test_result[183]: "511" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"511",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	511,	/* JSON floating point value in long double form */
    },

    /* test_result[184]: "511.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"511.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	511,	/* JSON floating point value in long double form */
    },

    /* test_result[185]: "511.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"511.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	511,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	511,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	511,	/* JSON floating point value in long double form */
    },

    /* test_result[186]: "512" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"512",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	512,	/* JSON floating point value in long double form */
    },

    /* test_result[187]: "512.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"512.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	512,	/* JSON floating point value in long double form */
    },

    /* test_result[188]: "512.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"512.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	512,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	512,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	512,	/* JSON floating point value in long double form */
    },

    /* test_result[189]: "513" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"513",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	513,	/* JSON floating point value in long double form */
    },

    /* test_result[190]: "513.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"513.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	513,	/* JSON floating point value in long double form */
    },

    /* test_result[191]: "513.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"513.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	513,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	513,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	513,	/* JSON floating point value in long double form */
    },

    /* test_result[192]: "514" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"514",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	514,	/* JSON floating point value in long double form */
    },

    /* test_result[193]: "514.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"514.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	514,	/* JSON floating point value in long double form */
    },

    /* test_result[194]: "514.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"514.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	514,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	514,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	514,	/* JSON floating point value in long double form */
    },

    /* test_result[195]: "32766" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32766",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32766,	/* JSON floating point value in long double form */
    },

    /* test_result[196]: "32766.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32766.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32766,	/* JSON floating point value in long double form */
    },

    /* test_result[197]: "32766.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32766.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32766,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32766,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32766,	/* JSON floating point value in long double form */
    },

    /* test_result[198]: "32767" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32767",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32767,	/* JSON floating point value in long double form */
    },

    /* test_result[199]: "32767.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32767.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32767,	/* JSON floating point value in long double form */
    },

    /* test_result[200]: "32767.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32767.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32767,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32767,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32767,	/* JSON floating point value in long double form */
    },

    /* test_result[201]: "32768" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32768",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32768,	/* JSON floating point value in long double form */
    },

    /* test_result[202]: "32768.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32768.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32768,	/* JSON floating point value in long double form */
    },

    /* test_result[203]: "32768.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32768.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32768,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32768,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32768,	/* JSON floating point value in long double form */
    },

    /* test_result[204]: "32769" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32769",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32769,	/* JSON floating point value in long double form */
    },

    /* test_result[205]: "32769.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32769.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32769,	/* JSON floating point value in long double form */
    },

    /* test_result[206]: "32769.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32769.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32769,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32769,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32769,	/* JSON floating point value in long double form */
    },

    /* test_result[207]: "32770" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32770",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32770,	/* JSON floating point value in long double form */
    },

    /* test_result[208]: "32770.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32770.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32770,	/* JSON floating point value in long double form */
    },

    /* test_result[209]: "32770.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"32770.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	32770,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	32770,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	32770,	/* JSON floating point value in long double form */
    },

    /* test_result[210]: "65534" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65534",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65534,	/* JSON floating point value in long double form */
    },

    /* test_result[211]: "65534.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65534.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65534,	/* JSON floating point value in long double form */
    },

    /* test_result[212]: "65534.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65534.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65534,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65534,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65534,	/* JSON floating point value in long double form */
    },

    /* test_result[213]: "65535" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65535",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65535,	/* JSON floating point value in long double form */
    },

    /* test_result[214]: "65535.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65535.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65535,	/* JSON floating point value in long double form */
    },

    /* test_result[215]: "65535.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65535.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65535,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65535,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65535,	/* JSON floating point value in long double form */
    },

    /* test_result[216]: "65536" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65536",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65536,	/* JSON floating point value in long double form */
    },

    /* test_result[217]: "65536.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65536.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65536,	/* JSON floating point value in long double form */
    },

    /* test_result[218]: "65536.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65536.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65536,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65536,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65536,	/* JSON floating point value in long double form */
    },

    /* test_result[219]: "65537" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65537",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65537,	/* JSON floating point value in long double form */
    },

    /* test_result[220]: "65537.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65537.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65537,	/* JSON floating point value in long double form */
    },

    /* test_result[221]: "65537.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65537.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65537,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65537,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65537,	/* JSON floating point value in long double form */
    },

    /* test_result[222]: "65538" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65538",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65538,	/* JSON floating point value in long double form */
    },

    /* test_result[223]: "65538.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65538.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65538,	/* JSON floating point value in long double form */
    },

    /* test_result[224]: "65538.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"65538.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	65538,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	65538,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	65538,	/* JSON floating point value in long double form */
    },

    /* test_result[225]: "131070" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131070",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131070,	/* JSON floating point value in long double form */
    },

    /* test_result[226]: "131070.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131070.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131070,	/* JSON floating point value in long double form */
    },

    /* test_result[227]: "131070.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131070.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131070,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131070,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131070,	/* JSON floating point value in long double form */
    },

    /* test_result[228]: "131071" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131071",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131071,	/* JSON floating point value in long double form */
    },

    /* test_result[229]: "131071.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131071.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131071,	/* JSON floating point value in long double form */
    },

    /* test_result[230]: "131071.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131071.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131071,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131071,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131071,	/* JSON floating point value in long double form */
    },

    /* test_result[231]: "131072" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131072",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131072,	/* JSON floating point value in long double form */
    },

    /* test_result[232]: "131072.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131072.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131072,	/* JSON floating point value in long double form */
    },

    /* test_result[233]: "131072.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131072.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131072,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131072,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131072,	/* JSON floating point value in long double form */
    },

    /* test_result[234]: "131073" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131073",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131073,	/* JSON floating point value in long double form */
    },

    /* test_result[235]: "131073.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131073.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131073,	/* JSON floating point value in long double form */
    },

    /* test_result[236]: "131073.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131073.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131073,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131073,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131073,	/* JSON floating point value in long double form */
    },

    /* test_result[237]: "131074" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131074",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131074,	/* JSON floating point value in long double form */
    },

    /* test_result[238]: "131074.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131074.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131074,	/* JSON floating point value in long double form */
    },

    /* test_result[239]: "131074.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"131074.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	131074,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	131074,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	131074,	/* JSON floating point value in long double form */
    },

    /* test_result[240]: "2147483646" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483646",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[241]: "2147483646.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483646.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[242]: "2147483646.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483646.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646,	/* JSON floating point value in long double form */
    },

    /* test_result[243]: "2147483647" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483647",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[244]: "2147483647.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483647.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[245]: "2147483647.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483647.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647,	/* JSON floating point value in long double form */
    },

    /* test_result[246]: "2147483648" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483648",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[247]: "2147483648.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483648.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[248]: "2147483648.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483648.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648,	/* JSON floating point value in long double form */
    },

    /* test_result[249]: "2147483649" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483649",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[250]: "2147483649.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483649.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[251]: "2147483649.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483649.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649,	/* JSON floating point value in long double form */
    },

    /* test_result[252]: "2147483650" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483650",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[253]: "2147483650.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483650.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[254]: "2147483650.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"2147483650.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650,	/* JSON floating point value in long double form */
    },

    /* test_result[255]: "4294967294" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967294",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[256]: "4294967294.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967294.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[257]: "4294967294.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967294.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294,	/* JSON floating point value in long double form */
    },

    /* test_result[258]: "4294967295" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967295",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[259]: "4294967295.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967295.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[260]: "4294967295.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967295.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295,	/* JSON floating point value in long double form */
    },

    /* test_result[261]: "4294967296" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967296",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[262]: "4294967296.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967296.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[263]: "4294967296.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967296.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296,	/* JSON floating point value in long double form */
    },

    /* test_result[264]: "4294967297" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967297",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[265]: "4294967297.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967297.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[266]: "4294967297.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967297.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297,	/* JSON floating point value in long double form */
    },

    /* test_result[267]: "4294967298" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967298",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[268]: "4294967298.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967298.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[269]: "4294967298.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"4294967298.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298,	/* JSON floating point value in long double form */
    },

    /* test_result[270]: "8589934590" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934590",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[271]: "8589934590.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934590.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[272]: "8589934590.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934590.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590,	/* JSON floating point value in long double form */
    },

    /* test_result[273]: "8589934591" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934591",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[274]: "8589934591.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934591.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[275]: "8589934591.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934591.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591,	/* JSON floating point value in long double form */
    },

    /* test_result[276]: "8589934592" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934592",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[277]: "8589934592.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934592.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[278]: "8589934592.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934592.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592,	/* JSON floating point value in long double form */
    },

    /* test_result[279]: "8589934593" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934593",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[280]: "8589934593.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934593.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[281]: "8589934593.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934593.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593,	/* JSON floating point value in long double form */
    },

    /* test_result[282]: "8589934594" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934594",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594,	/* JSON floating point value in long double form */
    },

    /* test_result[283]: "8589934594.0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934594.0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594,	/* JSON floating point value in long double form */
    },

    /* test_result[284]: "8589934594.0e0" */
    {
	/* malloced JSON floating point string trimmed if needed, that was converted */
	"8589934594.0e0",

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594,	/* JSON floating point value in double form */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594,	/* JSON floating point value in long double form */
    },

};
#endif /* JFLOAT_TEST_ENABLED */
