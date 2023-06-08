/*
 * jprint_test - test functions for the jprint tool
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include "jprint_test.h"

/* jprint_run_tests	- run test functions
 *
 * given:
 *
 *	void	    - no args: this function is selfish :-)
 *
 * Returns false if any test failed. Will only return after all tests are run.
 */
bool
jprint_run_tests(void)
{
    struct jprint_number number;    /* number range */
    bool test = false;		    /* whether current test passes or fails */
    bool okay = true;	    /* if any test fails set to true, is return value */
    uintmax_t bits = 0;	    /* for bits tests */

    /* set up exact match of 5 */
    jprint_parse_number_range("-l", "5", &number);

    /* make sure number matches exactly */
    test = jprint_test_number_range_opts(true, 5, &number);
    if (!test) {
	okay = false;
    }

    /* make sure number does NOT match */
    test = jprint_test_number_range_opts(false, 6, &number);
    if (!test) {
	okay = false;
    }

    /* set up inclusive range of >= 5 && <= 10 */
    jprint_parse_number_range("-l", "5:10", &number);
    /* make sure that number is in the range >= 5 && <= 10 */
    test = jprint_test_number_range_opts(true, 6, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number is NOT in the range >= 5 && <= 10 due to >= max */
    test = jprint_test_number_range_opts(false, 11, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number is NOT in the range >= 5 && <= 10 due to < min */
    test = jprint_test_number_range_opts(false, 4, &number);
    if (!test) {
	okay = false;
    }

    /* set up minimum number */
    jprint_parse_number_range("-l", "10:", &number);
    /* make sure that number 10 is in the range >= 10 */
    test = jprint_test_number_range_opts(true, 10, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number 11 is in the range >= 10 */
    test = jprint_test_number_range_opts(true, 11, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number 9 is NOT >= 10 */
    test = jprint_test_number_range_opts(false, 9, &number);
    if (!test) {
	okay = false;
    }

    /* set up maximum number */
    jprint_parse_number_range("-l", ":10", &number);
    /* make sure that number 10 is in the range <= 10 */
    test = jprint_test_number_range_opts(true, 10, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number 9 is in the range <= 10 */
    test = jprint_test_number_range_opts(true, 9, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number 11 is NOT <= 10 */
    test = jprint_test_number_range_opts(false, 11, &number);
    if (!test) {
	okay = false;
    }

    /* now check bits */

    /* set bits to JPRINT_PRINT_BOTH */
    bits = JPRINT_PRINT_BOTH;

    /* check that JPRINT_PRINT_BOTH is equal to bits */
    test = jprint_test_bits(true, bits, jprint_print_name_value, "JPRINT_PRINT_BOTH");
    if (!test) {
	okay = false;
    }

    /* set bits to JPRINT_PRINT_NAME */
    bits = JPRINT_PRINT_NAME;
    /* check that only JPRINT_PRINT_NAME is set: both and value are not set */
    test = jprint_test_bits(true, bits, jprint_print_name, "JPRINT_PRINT_NAME") && 
	   jprint_test_bits(false, bits, jprint_print_value, "JPRINT_PRINT_VALUE") &&
	   jprint_test_bits(false, bits, jprint_print_name_value, "JPRINT_PRINT_BOTH");

    if (!test) {
	okay = false;
    }
    /* set bits to JPRINT_PRINT_VALUE */
    bits = JPRINT_PRINT_VALUE;
    /* check that only JPRINT_PRINT_VALUE is set: both and name are not set */
    test = jprint_test_bits(true, bits, jprint_print_value, "JPRINT_PRINT_VALUE") && 
	   jprint_test_bits(false, bits, jprint_print_name, "JPRINT_PRINT_NAME") &&
	   jprint_test_bits(false, bits, jprint_print_name_value, "JPRINT_PRINT_BOTH");

    if (!test) {
	okay = false;
    }

    return okay;
}
/* jprint_test_number_range_opts
 *
 * Test that the number range functionality works okay.
 *
 * given:
 *
 *	option	     	option that this is testing
 *	expected     	whether test should return true or false
 *	number		number to test
 *	range		range to verify number against
 *
 * Returns true if test is okay.
 *
 * NOTE: this will not return on NULL pointers.
 */
bool
jprint_test_number_range_opts(bool expected, intmax_t number, struct jprint_number *range)
{
    bool test = false;	    /* result of test */

    if (range == NULL) {
	err(15, __func__, "NULL range, cannot test");
	not_reached();
    }

    test = jprint_number_in_range(number, range);
    print("in function %s: expects %s: ", __func__, expected?"success":"failure");
    if (range->exact) {
	print("expect exact match for number %jd: ", number);
    } else if (range->range.inclusive) {
	print("expect number %jd to be >= %jd && <= %jd: ", number, range->range.min, range->range.max);
    } else if (range->range.greater_than_equal) {
	print("expect number %jd to be >= %ju: ", number, range->range.min);
    } else if (range->range.less_than_equal) {
	print("expect number %jd to be <= %jd: ", number, range->range.max);
    }

    print("test %s\n", expected == test?"OK":"failed");

    return expected == test;
}

/* jprint_test_bits    -	test bits code
 *
 * given:
 *
 *	expected	- whether test should fail or not
 *	set_bits	- the bits actually set
 *	check_func	- pointer to function of appropriate check
 *	name		- name of bits to check
 *
 * Returns true if the test succeeds otherwise false.
 *
 * NOTE: this function will not return on NULL function pointer or NULL name.
 */
bool
jprint_test_bits(bool expected, uintmax_t set_bits, bool (*check_func)(uintmax_t), const char *name)
{
    bool okay = true;	/* assume test will pass */
    bool test = false;	/* return value of function call */

    /* firewall */
    if (check_func == NULL) {
	err(13, __func__, "NULL check_func");
	not_reached();
    }
    print("in function %s: expects %s: ", __func__, expected?"success":"failure");
    print("expect bits: %s: ", name);
    test = check_func(set_bits);

    print("test %s\n", expected == test?"OK":"failed");
    if (expected != test) {
	okay = false;
    }

    return okay;
}
