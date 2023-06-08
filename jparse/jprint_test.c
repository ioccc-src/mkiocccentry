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
 * Returns true if any test failed. Will only return after all tests are run.
 */
bool
jprint_run_tests(void)
{
    struct jprint_number number;    /* number range */
    bool test = false;		    /* whether current test passes or fails */
    bool okay = true;	    /* if any test fails set to true, is return value */

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

    if (expected == test) {
	print("test %s\n", expected == test?"OK":"failed");
    }

    return expected == test;
}
