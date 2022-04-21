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
    "-8589934594.1",
    "-8589934594.2e0",
    "-8589934593",
    "-8589934593.1",
    "-8589934593.2e0",
    "-8589934592",
    "-8589934592.1",
    "-8589934592.2e0",
    "-8589934591",
    "-8589934591.1",
    "-8589934591.2e0",
    "-8589934590",
    "-8589934590.1",
    "-8589934590.2e0",
    "-4294967298",
    "-4294967298.1",
    "-4294967298.2e0",
    "-4294967297",
    "-4294967297.1",
    "-4294967297.2e0",
    "-4294967296",
    "-4294967296.1",
    "-4294967296.2e0",
    "-4294967295",
    "-4294967295.1",
    "-4294967295.2e0",
    "-4294967294",
    "-4294967294.1",
    "-4294967294.2e0",
    "-2147483650",
    "-2147483650.1",
    "-2147483650.2e0",
    "-2147483649",
    "-2147483649.1",
    "-2147483649.2e0",
    "-2147483648",
    "-2147483648.1",
    "-2147483648.2e0",
    "-2147483647",
    "-2147483647.1",
    "-2147483647.2e0",
    "-2147483646",
    "-2147483646.1",
    "-2147483646.2e0",
    "-131074",
    "-131074.1",
    "-131074.2e0",
    "-131073",
    "-131073.1",
    "-131073.2e0",
    "-131072",
    "-131072.1",
    "-131072.2e0",
    "-131071",
    "-131071.1",
    "-131071.2e0",
    "-131070",
    "-131070.1",
    "-131070.2e0",
    "-65538",
    "-65538.1",
    "-65538.2e0",
    "-65537",
    "-65537.1",
    "-65537.2e0",
    "-65536",
    "-65536.1",
    "-65536.2e0",
    "-65535",
    "-65535.1",
    "-65535.2e0",
    "-65534",
    "-65534.1",
    "-65534.2e0",
    "-32770",
    "-32770.1",
    "-32770.2e0",
    "-32769",
    "-32769.1",
    "-32769.2e0",
    "-32768",
    "-32768.1",
    "-32768.2e0",
    "-32767",
    "-32767.1",
    "-32767.2e0",
    "-32766",
    "-32766.1",
    "-32766.2e0",
    "-514",
    "-514.1",
    "-514.2e0",
    "-513",
    "-513.1",
    "-513.2e0",
    "-512",
    "-512.1",
    "-512.2e0",
    "-511",
    "-511.1",
    "-511.2e0",
    "-510",
    "-510.1",
    "-510.2e0",
    "-258",
    "-258.1",
    "-258.2e0",
    "-257",
    "-257.1",
    "-257.2e0",
    "-256",
    "-256.1",
    "-256.2e0",
    "-255",
    "-255.1",
    "-255.2e0",
    "-254",
    "-254.1",
    "-254.2e0",
    "-130",
    "-130.1",
    "-130.2e0",
    "-129",
    "-129.1",
    "-129.2e0",
    "-128",
    "-128.1",
    "-128.2e0",
    "-127",
    "-127.1",
    "-127.2e0",
    "-126",
    "-126.1",
    "-126.2e0",
    "-2",
    "-2.1",
    "-2.2e0",
    "-1",
    "-1.1",
    "-1.2e0",
    "0",
    "0.1",
    "0.2e0",
    "1",
    "1.1",
    "1.2e0",
    "2",
    "2.1",
    "2.2e0",
    "126",
    "126.1",
    "126.2e0",
    "127",
    "127.1",
    "127.2e0",
    "128",
    "128.1",
    "128.2e0",
    "129",
    "129.1",
    "129.2e0",
    "130",
    "130.1",
    "130.2e0",
    "254",
    "254.1",
    "254.2e0",
    "255",
    "255.1",
    "255.2e0",
    "256",
    "256.1",
    "256.2e0",
    "257",
    "257.1",
    "257.2e0",
    "258",
    "258.1",
    "258.2e0",
    "510",
    "510.1",
    "510.2e0",
    "511",
    "511.1",
    "511.2e0",
    "512",
    "512.1",
    "512.2e0",
    "513",
    "513.1",
    "513.2e0",
    "514",
    "514.1",
    "514.2e0",
    "32766",
    "32766.1",
    "32766.2e0",
    "32767",
    "32767.1",
    "32767.2e0",
    "32768",
    "32768.1",
    "32768.2e0",
    "32769",
    "32769.1",
    "32769.2e0",
    "32770",
    "32770.1",
    "32770.2e0",
    "65534",
    "65534.1",
    "65534.2e0",
    "65535",
    "65535.1",
    "65535.2e0",
    "65536",
    "65536.1",
    "65536.2e0",
    "65537",
    "65537.1",
    "65537.2e0",
    "65538",
    "65538.1",
    "65538.2e0",
    "131070",
    "131070.1",
    "131070.2e0",
    "131071",
    "131071.1",
    "131071.2e0",
    "131072",
    "131072.1",
    "131072.2e0",
    "131073",
    "131073.1",
    "131073.2e0",
    "131074",
    "131074.1",
    "131074.2e0",
    "2147483646",
    "2147483646.1",
    "2147483646.2e0",
    "2147483647",
    "2147483647.1",
    "2147483647.2e0",
    "2147483648",
    "2147483648.1",
    "2147483648.2e0",
    "2147483649",
    "2147483649.1",
    "2147483649.2e0",
    "2147483650",
    "2147483650.1",
    "2147483650.2e0",
    "4294967294",
    "4294967294.1",
    "4294967294.2e0",
    "4294967295",
    "4294967295.1",
    "4294967295.2e0",
    "4294967296",
    "4294967296.1",
    "4294967296.2e0",
    "4294967297",
    "4294967297.1",
    "4294967297.2e0",
    "4294967298",
    "4294967298.1",
    "4294967298.2e0",
    "8589934590",
    "8589934590.1",
    "8589934590.2e0",
    "8589934591",
    "8589934591.1",
    "8589934591.2e0",
    "8589934592",
    "8589934592.1",
    "8589934592.2e0",
    "8589934593",
    "8589934593.1",
    "8589934593.2e0",
    "8589934594",
    "8589934594.1",
    "8589934594.2e0",
    NULL
};

struct json_floating test_result[TEST_COUNT] = {
    /* test_result[0]: "-8589934594" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934594",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[1]: "-8589934594.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934594.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[2]: "-8589934594.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934594.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934594.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934594.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[3]: "-8589934593" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934593",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[4]: "-8589934593.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934593.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[5]: "-8589934593.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934593.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934593.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934593.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[6]: "-8589934592" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934592",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[7]: "-8589934592.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934592.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[8]: "-8589934592.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934592.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934592.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934592.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[9]: "-8589934591" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934591",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[10]: "-8589934591.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934591.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[11]: "-8589934591.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934591.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934591.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934591.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[12]: "-8589934590" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934590",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[13]: "-8589934590.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934590.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[14]: "-8589934590.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-8589934590.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-8589934590.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-8589934590.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[15]: "-4294967298" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967298",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[16]: "-4294967298.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967298.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[17]: "-4294967298.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967298.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967298.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967298.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[18]: "-4294967297" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967297",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[19]: "-4294967297.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967297.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[20]: "-4294967297.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967297.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967297.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967297.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[21]: "-4294967296" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967296",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[22]: "-4294967296.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967296.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[23]: "-4294967296.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967296.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967296.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967296.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[24]: "-4294967295" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967295",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[25]: "-4294967295.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967295.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[26]: "-4294967295.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967295.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967295.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967295.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[27]: "-4294967294" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967294",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[28]: "-4294967294.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967294.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[29]: "-4294967294.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-4294967294.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-4294967294.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-4294967294.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[30]: "-2147483650" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483650",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[31]: "-2147483650.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483650.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[32]: "-2147483650.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483650.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483650.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483650.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[33]: "-2147483649" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483649",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[34]: "-2147483649.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483649.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[35]: "-2147483649.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483649.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483649.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483649.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[36]: "-2147483648" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483648",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[37]: "-2147483648.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483648.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[38]: "-2147483648.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483648.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483648.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483648.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[39]: "-2147483647" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483647",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[40]: "-2147483647.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483647.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[41]: "-2147483647.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483647.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483647.200000047684,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483647.200000047684,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[42]: "-2147483646" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483646",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[43]: "-2147483646.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483646.1",

	13,	/* length of original JSON floating point string */
	13,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[44]: "-2147483646.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2147483646.2e0",

	15,	/* length of original JSON floating point string */
	15,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2147483646.200000047684,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2147483646.200000047684,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[45]: "-131074" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131074",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131074,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131074,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[46]: "-131074.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131074.1",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131074.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131074.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[47]: "-131074.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131074.2e0",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131074.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131074.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131074.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[48]: "-131073" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131073",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131073,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131073,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[49]: "-131073.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131073.1",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131073.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131073.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[50]: "-131073.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131073.2e0",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131073.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131073.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131073.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[51]: "-131072" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131072",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131072,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131072,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[52]: "-131072.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131072.1",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131072.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131072.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[53]: "-131072.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131072.2e0",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131072.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131072.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131072.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[54]: "-131071" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131071",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131071,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131071,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[55]: "-131071.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131071.1",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131071.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131071.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[56]: "-131071.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131071.2e0",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131071.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131071.1999999999970896,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131071.1999999999970896,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[57]: "-131070" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131070",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131070,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131070,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[58]: "-131070.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131070.1",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131070.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131070.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[59]: "-131070.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-131070.2e0",

	11,	/* length of original JSON floating point string */
	11,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-131070.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-131070.1999999999970896,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-131070.1999999999970896,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[60]: "-65538" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65538",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65538,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65538,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[61]: "-65538.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65538.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65538.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65538.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[62]: "-65538.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65538.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65538.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65538.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65538.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[63]: "-65537" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65537",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65537,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65537,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[64]: "-65537.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65537.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65537.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65537.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[65]: "-65537.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65537.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65537.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65537.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65537.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[66]: "-65536" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65536",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65536,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65536,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[67]: "-65536.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65536.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65536.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65536.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[68]: "-65536.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65536.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65536.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65536.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65536.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[69]: "-65535" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65535",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65535,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65535,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[70]: "-65535.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65535.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65535.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65535.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[71]: "-65535.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65535.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65535.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65535.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65535.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[72]: "-65534" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65534",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65534,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65534,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[73]: "-65534.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65534.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65534.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65534.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[74]: "-65534.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-65534.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-65534.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-65534.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-65534.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[75]: "-32770" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32770",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32770,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32770,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[76]: "-32770.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32770.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32770.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32770.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[77]: "-32770.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32770.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32770.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32770.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32770.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[78]: "-32769" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32769",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32769,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32769,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[79]: "-32769.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32769.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32769.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32769.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[80]: "-32769.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32769.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32769.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32769.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32769.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[81]: "-32768" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32768",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32768,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32768,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[82]: "-32768.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32768.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32768.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32768.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[83]: "-32768.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32768.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32768.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32768.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32768.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[84]: "-32767" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32767",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32767,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32767,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[85]: "-32767.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32767.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32767.099609375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32767.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[86]: "-32767.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32767.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32767.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32767.2000000000007276,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32767.2000000000007276,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[87]: "-32766" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32766",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32766,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32766,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[88]: "-32766.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32766.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32766.099609375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32766.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[89]: "-32766.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-32766.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-32766.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-32766.2000000000007276,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-32766.2000000000007276,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[90]: "-514" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-514",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-514,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-514,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-514,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[91]: "-514.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-514.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-514.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-514.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-514.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[92]: "-514.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-514.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-514.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-514.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-514.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[93]: "-513" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-513",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-513,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-513,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-513,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[94]: "-513.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-513.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-513.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-513.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-513.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[95]: "-513.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-513.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-513.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-513.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-513.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[96]: "-512" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-512",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-512,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-512,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-512,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[97]: "-512.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-512.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-512.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-512.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-512.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[98]: "-512.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-512.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-512.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-512.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-512.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[99]: "-511" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-511",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-511,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-511,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-511,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[100]: "-511.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-511.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-511.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-511.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-511.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[101]: "-511.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-511.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-511.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-511.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-511.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[102]: "-510" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-510",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-510,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-510,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-510,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[103]: "-510.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-510.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-510.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-510.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-510.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[104]: "-510.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-510.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-510.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-510.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-510.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[105]: "-258" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-258",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-258,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-258,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-258,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[106]: "-258.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-258.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-258.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-258.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-258.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[107]: "-258.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-258.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-258.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-258.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-258.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[108]: "-257" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-257",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-257,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-257,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-257,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[109]: "-257.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-257.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-257.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-257.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-257.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[110]: "-257.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-257.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-257.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-257.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-257.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[111]: "-256" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-256",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-256,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-256,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-256,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[112]: "-256.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-256.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-256.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-256.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-256.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[113]: "-256.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-256.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-256.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-256.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-256.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[114]: "-255" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-255",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-255,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-255,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-255,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[115]: "-255.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-255.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-255.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-255.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-255.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[116]: "-255.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-255.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-255.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-255.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-255.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[117]: "-254" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-254",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-254,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-254,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-254,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[118]: "-254.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-254.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-254.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-254.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-254.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[119]: "-254.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-254.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-254.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-254.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-254.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[120]: "-130" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-130",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-130,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-130,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-130,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[121]: "-130.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-130.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-130.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-130.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-130.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[122]: "-130.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-130.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-130.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-130.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-130.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[123]: "-129" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-129",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-129,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-129,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-129,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[124]: "-129.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-129.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-129.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-129.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-129.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[125]: "-129.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-129.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-129.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-129.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-129.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[126]: "-128" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-128",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-128,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-128,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-128,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[127]: "-128.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-128.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-128.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-128.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-128.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[128]: "-128.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-128.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-128.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-128.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-128.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[129]: "-127" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-127",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-127,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-127,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-127,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[130]: "-127.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-127.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-127.09999847412109375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-127.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-127.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[131]: "-127.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-127.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-127.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-127.2000000000000028422,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-127.2000000000000028422,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[132]: "-126" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-126",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-126,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-126,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-126,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[133]: "-126.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-126.1",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-126.09999847412109375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-126.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-126.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[134]: "-126.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-126.2e0",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-126.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-126.2000000000000028422,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-126.2000000000000028422,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[135]: "-2" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2",

	2,	/* length of original JSON floating point string */
	2,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[136]: "-2.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2.1",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2.099999904632568359375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2.100000000000000088818,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2.100000000000000088818,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[137]: "-2.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-2.2e0",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-2.200000047683715820312,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-2.200000000000000177636,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-2.200000000000000177636,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[138]: "-1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-1",

	2,	/* length of original JSON floating point string */
	2,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-1,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-1,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-1,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[139]: "-1.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-1.1",

	4,	/* length of original JSON floating point string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-1.100000023841857910156,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-1.100000000000000088818,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-1.100000000000000088818,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[140]: "-1.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"-1.2e0",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	true,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	-1.200000047683715820312,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	-1.199999999999999955591,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	-1.199999999999999955591,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[141]: "0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"0",

	1,	/* length of original JSON floating point string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	0,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	0,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	0,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[142]: "0.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"0.1",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	0.1000000014901161193848,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	0.1000000000000000055511,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	0.1000000000000000055511,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[143]: "0.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"0.2e0",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	0.2000000029802322387695,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	0.2000000000000000111022,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	0.2000000000000000111022,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[144]: "1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"1",

	1,	/* length of original JSON floating point string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	1,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	1,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	1,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[145]: "1.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"1.1",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	1.100000023841857910156,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	1.100000000000000088818,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	1.100000000000000088818,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[146]: "1.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"1.2e0",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	1.200000047683715820312,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	1.199999999999999955591,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	1.199999999999999955591,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[147]: "2" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2",

	1,	/* length of original JSON floating point string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[148]: "2.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2.1",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2.099999904632568359375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2.100000000000000088818,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2.100000000000000088818,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[149]: "2.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2.2e0",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2.200000047683715820312,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2.200000000000000177636,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2.200000000000000177636,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[150]: "126" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"126",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	126,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	126,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	126,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[151]: "126.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"126.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	126.09999847412109375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	126.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	126.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[152]: "126.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"126.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	126.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	126.2000000000000028422,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	126.2000000000000028422,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[153]: "127" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"127",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	127,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	127,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	127,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[154]: "127.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"127.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	127.09999847412109375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	127.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	127.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[155]: "127.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"127.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	127.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	127.2000000000000028422,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	127.2000000000000028422,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[156]: "128" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"128",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	128,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	128,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	128,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[157]: "128.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"128.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	128.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	128.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	128.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[158]: "128.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"128.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	128.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	128.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	128.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[159]: "129" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"129",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	129,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	129,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	129,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[160]: "129.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"129.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	129.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	129.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	129.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[161]: "129.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"129.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	129.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	129.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	129.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[162]: "130" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"130",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	130,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	130,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	130,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[163]: "130.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"130.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	130.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	130.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	130.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[164]: "130.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"130.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	130.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	130.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	130.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[165]: "254" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"254",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	254,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	254,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	254,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[166]: "254.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"254.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	254.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	254.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	254.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[167]: "254.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"254.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	254.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	254.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	254.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[168]: "255" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"255",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	255,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	255,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	255,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[169]: "255.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"255.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	255.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	255.0999999999999943157,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	255.0999999999999943157,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[170]: "255.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"255.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	255.1999969482421875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	255.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	255.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[171]: "256" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"256",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	256,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	256,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	256,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[172]: "256.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"256.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	256.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	256.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	256.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[173]: "256.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"256.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	256.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	256.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	256.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[174]: "257" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"257",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	257,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	257,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	257,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[175]: "257.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"257.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	257.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	257.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	257.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[176]: "257.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"257.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	257.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	257.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	257.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[177]: "258" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"258",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	258,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	258,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	258,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[178]: "258.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"258.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	258.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	258.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	258.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[179]: "258.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"258.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	258.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	258.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	258.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[180]: "510" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"510",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	510,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	510,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	510,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[181]: "510.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"510.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	510.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	510.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	510.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[182]: "510.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"510.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	510.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	510.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	510.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[183]: "511" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"511",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	511,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	511,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	511,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[184]: "511.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"511.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	511.100006103515625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	511.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	511.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[185]: "511.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"511.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	511.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	511.1999999999999886313,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	511.1999999999999886313,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[186]: "512" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"512",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	512,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	512,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	512,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[187]: "512.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"512.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	512.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	512.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	512.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[188]: "512.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"512.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	512.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	512.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	512.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[189]: "513" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"513",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	513,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	513,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	513,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[190]: "513.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"513.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	513.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	513.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	513.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[191]: "513.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"513.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	513.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	513.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	513.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[192]: "514" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"514",

	3,	/* length of original JSON floating point string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	514,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	514,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	514,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[193]: "514.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"514.1",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	514.0999755859375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	514.1000000000000227374,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	514.1000000000000227374,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[194]: "514.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"514.2e0",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	514.20001220703125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	514.2000000000000454747,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	514.2000000000000454747,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[195]: "32766" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32766",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32766,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32766,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32766,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[196]: "32766.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32766.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32766.099609375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32766.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32766.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[197]: "32766.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32766.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32766.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32766.2000000000007276,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32766.2000000000007276,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[198]: "32767" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32767",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32767,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32767,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32767,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[199]: "32767.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32767.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32767.099609375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32767.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32767.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[200]: "32767.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32767.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32767.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32767.2000000000007276,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32767.2000000000007276,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[201]: "32768" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32768",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32768,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32768,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32768,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[202]: "32768.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32768.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32768.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32768.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32768.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[203]: "32768.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32768.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32768.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32768.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32768.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[204]: "32769" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32769",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32769,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32769,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32769,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[205]: "32769.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32769.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32769.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32769.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32769.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[206]: "32769.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32769.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32769.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32769.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32769.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[207]: "32770" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32770",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32770,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32770,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32770,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[208]: "32770.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32770.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32770.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32770.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32770.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[209]: "32770.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"32770.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	32770.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	32770.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	32770.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[210]: "65534" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65534",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65534,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65534,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65534,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[211]: "65534.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65534.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65534.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65534.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65534.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[212]: "65534.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65534.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65534.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65534.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65534.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[213]: "65535" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65535",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65535,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65535,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65535,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[214]: "65535.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65535.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65535.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65535.09999999999854481,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65535.09999999999854481,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[215]: "65535.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65535.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65535.19921875,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65535.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65535.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[216]: "65536" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65536",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65536,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65536,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65536,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[217]: "65536.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65536.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65536.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65536.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65536.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[218]: "65536.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65536.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65536.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65536.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65536.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[219]: "65537" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65537",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65537,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65537,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65537,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[220]: "65537.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65537.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65537.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65537.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65537.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[221]: "65537.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65537.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65537.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65537.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65537.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[222]: "65538" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65538",

	5,	/* length of original JSON floating point string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65538,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65538,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65538,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[223]: "65538.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65538.1",

	7,	/* length of original JSON floating point string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65538.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65538.10000000000582077,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65538.10000000000582077,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[224]: "65538.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"65538.2e0",

	9,	/* length of original JSON floating point string */
	9,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	65538.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	65538.19999999999708962,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	65538.19999999999708962,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[225]: "131070" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131070",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131070,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131070,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131070,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[226]: "131070.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131070.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131070.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131070.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131070.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[227]: "131070.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131070.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131070.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131070.1999999999970896,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131070.1999999999970896,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[228]: "131071" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131071",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131071,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131071,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131071,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[229]: "131071.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131071.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131071.1015625,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131071.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131071.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[230]: "131071.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131071.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131071.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131071.1999999999970896,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131071.1999999999970896,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[231]: "131072" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131072",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131072,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131072,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131072,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[232]: "131072.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131072.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131072.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131072.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131072.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[233]: "131072.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131072.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131072.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131072.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131072.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[234]: "131073" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131073",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131073,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131073,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131073,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[235]: "131073.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131073.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131073.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131073.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131073.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[236]: "131073.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131073.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131073.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131073.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131073.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[237]: "131074" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131074",

	6,	/* length of original JSON floating point string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131074,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131074,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131074,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[238]: "131074.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131074.1",

	8,	/* length of original JSON floating point string */
	8,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131074.09375,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131074.1000000000058208,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131074.1000000000058208,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[239]: "131074.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"131074.2e0",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	131074.203125,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	131074.2000000000116415,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	131074.2000000000116415,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[240]: "2147483646" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483646",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[241]: "2147483646.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483646.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[242]: "2147483646.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483646.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483646.200000047684,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483646.200000047684,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[243]: "2147483647" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483647",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[244]: "2147483647.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483647.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[245]: "2147483647.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483647.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483647.200000047684,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483647.200000047684,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[246]: "2147483648" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483648",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[247]: "2147483648.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483648.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[248]: "2147483648.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483648.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483648.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483648.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[249]: "2147483649" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483649",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[250]: "2147483649.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483649.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[251]: "2147483649.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483649.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483649.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483649.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[252]: "2147483650" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483650",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[253]: "2147483650.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483650.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[254]: "2147483650.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"2147483650.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	2147483648,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	2147483650.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	2147483650.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[255]: "4294967294" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967294",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[256]: "4294967294.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967294.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[257]: "4294967294.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967294.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967294.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967294.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[258]: "4294967295" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967295",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[259]: "4294967295.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967295.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295.099999904633,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295.099999904633,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[260]: "4294967295.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967295.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967295.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967295.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[261]: "4294967296" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967296",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[262]: "4294967296.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967296.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[263]: "4294967296.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967296.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967296.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967296.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[264]: "4294967297" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967297",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[265]: "4294967297.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967297.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[266]: "4294967297.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967297.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967297.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967297.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[267]: "4294967298" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967298",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[268]: "4294967298.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967298.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[269]: "4294967298.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"4294967298.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	4294967296,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	4294967298.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	4294967298.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[270]: "8589934590" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934590",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[271]: "8589934590.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934590.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[272]: "8589934590.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934590.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934590.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934590.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[273]: "8589934591" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934591",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[274]: "8589934591.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934591.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[275]: "8589934591.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934591.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934591.199999809265,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934591.199999809265,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[276]: "8589934592" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934592",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[277]: "8589934592.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934592.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[278]: "8589934592.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934592.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934592.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934592.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[279]: "8589934593" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934593",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[280]: "8589934593.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934593.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[281]: "8589934593.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934593.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934593.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934593.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[282]: "8589934594" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934594",

	10,	/* length of original JSON floating point string */
	10,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	true,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594,	/* JSON floating point value in double form */
	true,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594,	/* JSON floating point value in long double form */
	true,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[283]: "8589934594.1" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934594.1",

	12,	/* length of original JSON floating point string */
	12,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	false,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594.10000038147,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594.10000038147,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

    /* test_result[284]: "8589934594.2e0" */
    {
	/* allocated JSON floating point string, whitespace trimmed if needed */
	"8589934594.2e0",

	14,	/* length of original JSON floating point string */
	14,	/* length of as_str */

	true,	/* true ==> able to convert JSON floating point to some form of C floating point type */
	false,	/* true ==> value < 0 */
	true,	/* true ==> e notation used */

	true,	/* true ==> converted JSON floating point to C float */
	8589934592,	/* JSON floating point value in float form */
	false,	/* if float_sized == true, true ==> as_float is an integer */

	true,	/* true ==> converted JSON floating point to C double */
	8589934594.200000762939,	/* JSON floating point value in double form */
	false,	/* if double_sized == true, true ==> as_double is an integer */

	true,	/* true ==> converted JSON floating point to C long double */
	8589934594.200000762939,	/* JSON floating point value in long double form */
	false,	/* if float_sized == true, true ==> as_float is an integer */
    },

};
#endif /* JFLOAT_TEST_ENABLED */
