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
#if defined(JINT_TEST_ENABLED)
#define TEST_COUNT (65)

int const test_count = TEST_COUNT;

char *test_set[TEST_COUNT+1] = {
    "-131074",
    "-131073",
    "-131072",
    "-131071",
    "-131070",
    "-65538",
    "-65537",
    "-65536",
    "-65535",
    "-65534",
    "-32770",
    "-32769",
    "-32768",
    "-32767",
    "-32766",
    "-514",
    "-513",
    "-512",
    "-511",
    "-510",
    "-258",
    "-257",
    "-256",
    "-255",
    "-254",
    "-130",
    "-129",
    "-128",
    "-127",
    "-126",
    "-2",
    "-1",
    "0",
    "1",
    "2",
    "126",
    "127",
    "128",
    "129",
    "130",
    "254",
    "255",
    "256",
    "257",
    "258",
    "510",
    "511",
    "512",
    "513",
    "514",
    "32766",
    "32767",
    "32768",
    "32769",
    "32770",
    "65534",
    "65535",
    "65536",
    "65537",
    "65538",
    "131070",
    "131071",
    "131072",
    "131073",
    "131074",
    NULL
};

struct json_integer test_result[TEST_COUNT] = {
    /* test_result[0]: "-131074" */
    {
	"-131074",	/* allocated JSON integer string, whitespace trimmed if needed */

	7,	/* length of original JSON integer string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-131074,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-131074,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-131074,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-131074,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-131074,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-131074,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-131074,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-131074,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[1]: "-131073" */
    {
	"-131073",	/* allocated JSON integer string, whitespace trimmed if needed */

	7,	/* length of original JSON integer string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-131073,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-131073,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-131073,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-131073,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-131073,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-131073,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-131073,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-131073,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[2]: "-131072" */
    {
	"-131072",	/* allocated JSON integer string, whitespace trimmed if needed */

	7,	/* length of original JSON integer string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-131072,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-131072,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-131072,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-131072,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-131072,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-131072,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-131072,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-131072,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[3]: "-131071" */
    {
	"-131071",	/* allocated JSON integer string, whitespace trimmed if needed */

	7,	/* length of original JSON integer string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-131071,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-131071,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-131071,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-131071,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-131071,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-131071,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-131071,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-131071,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[4]: "-131070" */
    {
	"-131070",	/* allocated JSON integer string, whitespace trimmed if needed */

	7,	/* length of original JSON integer string */
	7,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-131070,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-131070,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-131070,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-131070,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-131070,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-131070,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-131070,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-131070,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[5]: "-65538" */
    {
	"-65538",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-65538,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-65538,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-65538,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-65538,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-65538,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-65538,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-65538,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-65538,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[6]: "-65537" */
    {
	"-65537",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-65537,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-65537,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-65537,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-65537,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-65537,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-65537,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-65537,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-65537,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[7]: "-65536" */
    {
	"-65536",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-65536,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-65536,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-65536,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-65536,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-65536,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-65536,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-65536,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-65536,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[8]: "-65535" */
    {
	"-65535",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-65535,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-65535,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-65535,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-65535,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-65535,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-65535,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-65535,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-65535,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[9]: "-65534" */
    {
	"-65534",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-65534,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-65534,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-65534,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-65534,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-65534,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-65534,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-65534,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-65534,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[10]: "-32770" */
    {
	"-32770",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-32770,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-32770,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-32770,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-32770,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-32770,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-32770,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-32770,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-32770,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[11]: "-32769" */
    {
	"-32769",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-32769,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-32769,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-32769,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-32769,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-32769,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-32769,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-32769,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-32769,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[12]: "-32768" */
    {
	"-32768",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-32768,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-32768,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-32768,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-32768,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-32768,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-32768,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-32768,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-32768,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-32768,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[13]: "-32767" */
    {
	"-32767",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-32767,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-32767,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-32767,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-32767,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-32767,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-32767,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-32767,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-32767,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-32767,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[14]: "-32766" */
    {
	"-32766",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-32766,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-32766,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-32766,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-32766,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-32766,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-32766,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-32766,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-32766,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-32766,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[15]: "-514" */
    {
	"-514",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-514,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-514,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-514,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-514,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-514,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-514,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-514,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-514,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-514,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[16]: "-513" */
    {
	"-513",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-513,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-513,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-513,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-513,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-513,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-513,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-513,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-513,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-513,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[17]: "-512" */
    {
	"-512",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-512,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-512,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-512,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-512,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-512,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-512,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-512,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-512,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-512,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[18]: "-511" */
    {
	"-511",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-511,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-511,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-511,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-511,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-511,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-511,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-511,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-511,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-511,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[19]: "-510" */
    {
	"-510",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-510,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-510,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-510,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-510,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-510,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-510,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-510,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-510,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-510,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[20]: "-258" */
    {
	"-258",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-258,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-258,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-258,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-258,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-258,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-258,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-258,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-258,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-258,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[21]: "-257" */
    {
	"-257",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-257,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-257,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-257,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-257,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-257,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-257,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-257,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-257,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-257,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[22]: "-256" */
    {
	"-256",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-256,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-256,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-256,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-256,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-256,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-256,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-256,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-256,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-256,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[23]: "-255" */
    {
	"-255",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-255,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-255,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-255,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-255,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-255,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-255,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-255,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-255,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-255,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[24]: "-254" */
    {
	"-254",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-254,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-254,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-254,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-254,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-254,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-254,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-254,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-254,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-254,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[25]: "-130" */
    {
	"-130",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-130,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-130,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-130,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-130,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-130,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-130,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-130,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-130,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-130,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[26]: "-129" */
    {
	"-129",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-129,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-129,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-129,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-129,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-129,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-129,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-129,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-129,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-129,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[27]: "-128" */
    {
	"-128",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	-128,	/* JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-128,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-128,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-128,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-128,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-128,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-128,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-128,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-128,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-128,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[28]: "-127" */
    {
	"-127",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	-127,	/* JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-127,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-127,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-127,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-127,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-127,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-127,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-127,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-127,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-127,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[29]: "-126" */
    {
	"-126",	/* allocated JSON integer string, whitespace trimmed if needed */

	4,	/* length of original JSON integer string */
	4,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	-126,	/* JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-126,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-126,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-126,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-126,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-126,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-126,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-126,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-126,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-126,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[30]: "-2" */
    {
	"-2",	/* allocated JSON integer string, whitespace trimmed if needed */

	2,	/* length of original JSON integer string */
	2,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	-2,	/* JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-2,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-2,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-2,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-2,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-2,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-2,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-2,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-2,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-2,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[31]: "-1" */
    {
	"-1",	/* allocated JSON integer string, whitespace trimmed if needed */

	2,	/* length of original JSON integer string */
	2,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	true,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	-1,	/* JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	-1,	/* JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	-1,	/* JSON integer value in int32_t form */

	false,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* no JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	-1,	/* JSON integer value in int64_t form */

	false,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* no JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	-1,	/* JSON integer value in int form */

	false,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* no JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	-1,	/* JSON integer value in long form */

	false,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* no JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	-1,	/* JSON integer value in long long form */

	false,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* no JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	-1,	/* JSON integer value in ssize_t form */

	false,	/* true ==> converted JSON integer to C size_t */
	0,	/* no JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	-1,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	-1,	/* JSON integer value in intmax_t form */

	false,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* no JSON integer value in uintmax_t form */
    },

    /* test_result[32]: "0" */
    {
	"0",	/* allocated JSON integer string, whitespace trimmed if needed */

	1,	/* length of original JSON integer string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	0,	/* JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	0,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	0,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	0,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	0,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	0,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	0,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	0,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	0,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	0,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	0,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	0,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	0,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	0,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	0,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	0,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	0,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[33]: "1" */
    {
	"1",	/* allocated JSON integer string, whitespace trimmed if needed */

	1,	/* length of original JSON integer string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	1,	/* JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	1,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	1,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	1,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	1,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	1,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	1,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	1,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	1,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	1,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	1,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	1,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	1,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	1,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	1,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	1,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	1,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	1,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	1,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[34]: "2" */
    {
	"2",	/* allocated JSON integer string, whitespace trimmed if needed */

	1,	/* length of original JSON integer string */
	1,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	2,	/* JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	2,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	2,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	2,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	2,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	2,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	2,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	2,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	2,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	2,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	2,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	2,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	2,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	2,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	2,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	2,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	2,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	2,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	2,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[35]: "126" */
    {
	"126",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	126,	/* JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	126,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	126,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	126,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	126,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	126,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	126,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	126,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	126,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	126,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	126,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	126,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	126,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	126,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	126,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	126,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	126,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	126,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	126,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[36]: "127" */
    {
	"127",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	true,	/* true ==> converted JSON integer to C int8_t */
	127,	/* JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	127,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	127,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	127,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	127,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	127,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	127,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	127,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	127,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	127,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	127,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	127,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	127,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	127,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	127,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	127,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	127,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	127,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	127,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[37]: "128" */
    {
	"128",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	128,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	128,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	128,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	128,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	128,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	128,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	128,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	128,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	128,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	128,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	128,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	128,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	128,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	128,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	128,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	128,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	128,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	128,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[38]: "129" */
    {
	"129",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	129,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	129,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	129,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	129,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	129,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	129,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	129,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	129,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	129,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	129,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	129,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	129,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	129,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	129,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	129,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	129,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	129,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	129,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[39]: "130" */
    {
	"130",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	130,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	130,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	130,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	130,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	130,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	130,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	130,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	130,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	130,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	130,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	130,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	130,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	130,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	130,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	130,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	130,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	130,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	130,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[40]: "254" */
    {
	"254",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	254,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	254,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	254,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	254,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	254,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	254,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	254,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	254,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	254,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	254,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	254,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	254,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	254,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	254,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	254,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	254,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	254,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	254,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[41]: "255" */
    {
	"255",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	true,	/* true ==> converted JSON integer to C uint8_t */
	255,	/* JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	255,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	255,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	255,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	255,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	255,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	255,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	255,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	255,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	255,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	255,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	255,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	255,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	255,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	255,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	255,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	255,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	255,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[42]: "256" */
    {
	"256",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	256,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	256,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	256,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	256,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	256,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	256,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	256,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	256,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	256,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	256,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	256,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	256,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	256,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	256,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	256,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	256,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	256,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[43]: "257" */
    {
	"257",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	257,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	257,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	257,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	257,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	257,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	257,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	257,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	257,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	257,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	257,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	257,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	257,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	257,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	257,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	257,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	257,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	257,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[44]: "258" */
    {
	"258",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	258,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	258,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	258,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	258,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	258,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	258,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	258,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	258,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	258,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	258,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	258,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	258,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	258,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	258,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	258,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	258,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	258,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[45]: "510" */
    {
	"510",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	510,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	510,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	510,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	510,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	510,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	510,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	510,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	510,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	510,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	510,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	510,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	510,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	510,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	510,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	510,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	510,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	510,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[46]: "511" */
    {
	"511",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	511,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	511,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	511,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	511,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	511,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	511,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	511,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	511,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	511,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	511,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	511,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	511,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	511,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	511,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	511,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	511,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	511,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[47]: "512" */
    {
	"512",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	512,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	512,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	512,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	512,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	512,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	512,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	512,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	512,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	512,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	512,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	512,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	512,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	512,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	512,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	512,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	512,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	512,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[48]: "513" */
    {
	"513",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	513,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	513,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	513,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	513,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	513,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	513,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	513,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	513,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	513,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	513,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	513,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	513,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	513,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	513,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	513,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	513,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	513,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[49]: "514" */
    {
	"514",	/* allocated JSON integer string, whitespace trimmed if needed */

	3,	/* length of original JSON integer string */
	3,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	514,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	514,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	514,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	514,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	514,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	514,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	514,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	514,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	514,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	514,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	514,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	514,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	514,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	514,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	514,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	514,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	514,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[50]: "32766" */
    {
	"32766",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	32766,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	32766,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	32766,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	32766,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	32766,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	32766,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	32766,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	32766,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	32766,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	32766,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	32766,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	32766,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	32766,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	32766,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	32766,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	32766,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	32766,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[51]: "32767" */
    {
	"32767",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	true,	/* true ==> converted JSON integer to C int16_t */
	32767,	/* JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	32767,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	32767,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	32767,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	32767,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	32767,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	32767,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	32767,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	32767,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	32767,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	32767,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	32767,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	32767,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	32767,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	32767,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	32767,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	32767,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[52]: "32768" */
    {
	"32768",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	32768,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	32768,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	32768,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	32768,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	32768,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	32768,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	32768,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	32768,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	32768,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	32768,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	32768,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	32768,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	32768,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	32768,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	32768,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	32768,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[53]: "32769" */
    {
	"32769",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	32769,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	32769,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	32769,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	32769,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	32769,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	32769,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	32769,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	32769,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	32769,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	32769,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	32769,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	32769,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	32769,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	32769,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	32769,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	32769,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[54]: "32770" */
    {
	"32770",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	32770,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	32770,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	32770,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	32770,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	32770,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	32770,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	32770,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	32770,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	32770,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	32770,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	32770,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	32770,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	32770,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	32770,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	32770,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	32770,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[55]: "65534" */
    {
	"65534",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	65534,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	65534,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	65534,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	65534,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	65534,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	65534,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	65534,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	65534,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	65534,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	65534,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	65534,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	65534,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	65534,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	65534,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	65534,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	65534,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[56]: "65535" */
    {
	"65535",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	true,	/* true ==> converted JSON integer to C uint16_t */
	65535,	/* JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	65535,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	65535,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	65535,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	65535,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	65535,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	65535,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	65535,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	65535,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	65535,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	65535,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	65535,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	65535,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	65535,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	65535,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	65535,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[57]: "65536" */
    {
	"65536",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	65536,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	65536,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	65536,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	65536,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	65536,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	65536,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	65536,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	65536,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	65536,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	65536,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	65536,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	65536,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	65536,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	65536,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	65536,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[58]: "65537" */
    {
	"65537",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	65537,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	65537,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	65537,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	65537,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	65537,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	65537,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	65537,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	65537,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	65537,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	65537,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	65537,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	65537,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	65537,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	65537,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	65537,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[59]: "65538" */
    {
	"65538",	/* allocated JSON integer string, whitespace trimmed if needed */

	5,	/* length of original JSON integer string */
	5,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	65538,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	65538,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	65538,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	65538,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	65538,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	65538,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	65538,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	65538,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	65538,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	65538,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	65538,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	65538,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	65538,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	65538,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	65538,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[60]: "131070" */
    {
	"131070",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	131070,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	131070,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	131070,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	131070,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	131070,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	131070,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	131070,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	131070,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	131070,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	131070,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	131070,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	131070,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	131070,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	131070,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	131070,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[61]: "131071" */
    {
	"131071",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	131071,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	131071,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	131071,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	131071,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	131071,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	131071,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	131071,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	131071,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	131071,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	131071,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	131071,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	131071,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	131071,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	131071,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	131071,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[62]: "131072" */
    {
	"131072",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	131072,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	131072,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	131072,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	131072,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	131072,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	131072,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	131072,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	131072,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	131072,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	131072,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	131072,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	131072,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	131072,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	131072,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	131072,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[63]: "131073" */
    {
	"131073",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	131073,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	131073,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	131073,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	131073,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	131073,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	131073,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	131073,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	131073,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	131073,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	131073,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	131073,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	131073,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	131073,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	131073,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	131073,	/* JSON integer value in uintmax_t form */
    },

    /* test_result[64]: "131074" */
    {
	"131074",	/* allocated JSON integer string, whitespace trimmed if needed */

	6,	/* length of original JSON integer string */
	6,	/* length of as_str */

	true,	/* true ==> able to convert JSON integer to some form of C integer type */
	false,	/* true ==> value < 0 */


	false,	/* true ==> converted JSON integer to C int8_t */
	0,	/* no JSON integer value in int8_t form */

	false,	/* true ==> converted JSON integer to C uint8_t */
	0,	/* no JSON integer value in uint8_t form */

	false,	/* true ==> converted JSON integer to C int16_t */
	0,	/* no JSON integer value in int16_t form */

	false,	/* true ==> converted JSON integer to C uint16_t */
	0,	/* no JSON integer value in uint16_t form */

	true,	/* true ==> converted JSON integer to C int32_t */
	131074,	/* JSON integer value in int32_t form */

	true,	/* true ==> converted JSON integer to C uint32_t */
	131074,	/* JSON integer value in uint32_t form */

	true,	/* true ==> converted JSON integer to C int64_t */
	131074,	/* JSON integer value in int64_t form */

	true,	/* true ==> converted JSON integer to C uint64_t */
	131074,	/* JSON integer value in uint64_t form */

	true,	/* true ==> converted JSON integer to C int */
	131074,	/* JSON integer value in int form */

	true,	/* true ==> converted JSON integer to C unsigned int */
	131074,	/* JSON integer value in unsigned int form */

	true,	/* true ==> converted JSON integer to C long */
	131074,	/* JSON integer value in long form */

	true,	/* true ==> converted JSON integer to C unsigned long */
	131074,	/* JSON integer value in unsigned long form */

	true,	/* true ==> converted JSON integer to C long long */
	131074,	/* JSON integer value in long long form */

	true,	/* true ==> converted JSON integer to C unsigned long long */
	131074,	/* JSON integer value in unsigned long long form */

	true,	/* true ==> converted JSON integer to C ssize_t */
	131074,	/* JSON integer value in ssize_t form */

	true,	/* true ==> converted JSON integer to C size_t */
	131074,	/* JSON integer value in size_t form */

	true,	/* true ==> converted JSON integer to C off_t */
	131074,	/* JSON integer value in off_t form */

	true,	/* true ==> converted JSON integer to C intmax_t */
	131074,	/* JSON integer value in intmax_t form */

	true,	/* true ==> converted JSON integer to C uintmax_t */
	131074,	/* JSON integer value in uintmax_t form */
    },

};
#endif /* JINT_TEST_ENABLED */
