/*
 * jnamval_test - test functions for the jnamval tool
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


#include "jnamval_test.h"

/* jnamval_run_tests	- run test functions
 *
 * given:
 *
 *	void	    - no args: this function is selfish :-)
 *
 * Returns false if any test failed. Will only return after all tests are run.
 */
bool
jnamval_run_tests(void)
{
    struct json_util_number number;    /* number range */
    bool test = false;		    /* whether current test passes or fails */
    bool okay = true;	    /* if any test fails set to true, is return value */
    uintmax_t bits = 0;	    /* for bits tests */

    /* set up exact match of 5 */
    json_util_parse_number_range("-l", "5", false, &number);

    /* make sure number matches exactly */
    test = jnamval_test_number_range_opts(true, 5, 10, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* make sure number does NOT match */
    test = jnamval_test_number_range_opts(false, 6, 7, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* set up inclusive range of >= 5 && <= 10 */
    json_util_parse_number_range("-l", "5:10", false, &number);
    /* make sure that number is in the range >= 5 && <= 10 */
    test = jnamval_test_number_range_opts(true, 6, 10, __LINE__, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number is NOT in the range >= 5 && <= 10 due to >= max */
    test = jnamval_test_number_range_opts(false, 11, 12, __LINE__, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number is NOT in the range >= 5 && <= 10 due to < min */
    test = jnamval_test_number_range_opts(false, 4, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /*
     * set up inclusive range of >= 5 && <= max - 3 (i.e. up through the third to
     * last match)
     */
    json_util_parse_number_range("-n", "5:-3", true, &number);
    /* make sure that number is in the range >= 5 && <= 10 - 3 */
    test = jnamval_test_number_range_opts(true, 7, 10, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number is NOT in the range >= 5 && <= 10 - 3 due to >=
     * total_matches
     */
    test = jnamval_test_number_range_opts(false, 11, 10, __LINE__, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number is NOT in the range >= 5 && <= 10 - 3 due to being
     * > total_matches - -max
     */
    test = jnamval_test_number_range_opts(false, 8, 10, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number is NOT in the range >= 5 && <= 10 due to < min */
    test = jnamval_test_number_range_opts(false, 4, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }


    /* set up minimum number */
    json_util_parse_number_range("-l", "10:", false, &number);
    /* make sure that number 10 is in the range >= 10 */
    test = jnamval_test_number_range_opts(true, 10, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number 11 is in the range >= 10 */
    test = jnamval_test_number_range_opts(true, 11, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number 9 is NOT >= 10 */
    test = jnamval_test_number_range_opts(false, 9, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* set up maximum number */
    json_util_parse_number_range("-l", ":10", false, &number);
    /* make sure that number 10 is in the range <= 10 */
    test = jnamval_test_number_range_opts(true, 10, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }
    /* make sure that number 9 is in the range <= 10 */
    test = jnamval_test_number_range_opts(true, 9, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* make sure that number 11 is NOT <= 10 */
    test = jnamval_test_number_range_opts(false, 11, 42, __LINE__, &number);
    if (!test) {
	okay = false;
    }

    /* now check bits */

    /* set bits to JNAMVAL_PRINT_BOTH */
    bits = jnamval_parse_print_option("both");

    /* check that JNAMVAL_PRINT_BOTH is equal to bits */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_print_both, "JNAMVAL_PRINT_BOTH");
    if (!test) {
	okay = false;
    }

    /* set bits to JNAMVAL_PRINT_NAME */
    bits = jnamval_parse_print_option("name");
    /* check that only JNAMVAL_PRINT_NAME is set: both and value are not set */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_print_name, "JNAMVAL_PRINT_NAME") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_value, "JNAMVAL_PRINT_VALUE") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_both, "JNAMVAL_PRINT_BOTH");

    if (!test) {
	okay = false;
    }
    /* set bits to JNAMVAL_PRINT_VALUE */
    bits = jnamval_parse_print_option("v");
    /* check that only JNAMVAL_PRINT_VALUE is set: both and name are not set */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_print_value, "JNAMVAL_PRINT_VALUE") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_name, "JNAMVAL_PRINT_NAME") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_both, "JNAMVAL_PRINT_BOTH");

    if (!test) {
	okay = false;
    }

    /* set bits to JNAMVAL_PRINT_JSON */
    bits = jnamval_parse_print_option("j");
    /* check that only JNAMVAL_PRINT_VALUE is set: both and name are not set */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_print_json, "JNAMVAL_PRINT_JSON") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_name, "JNAMVAL_PRINT_NAME") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_value, "JNAMVAL_PRINT_VALUE") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_both, "JNAMVAL_PRINT_BOTH");

    if (!test) {
	okay = false;
    }

    /* set bits to JNAMVAL_PRINT_JSON by full name */
    bits = jnamval_parse_print_option("json");
    /* check that only JNAMVAL_PRINT_VALUE is set: both and name are not set */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_print_json, "JNAMVAL_PRINT_JSON") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_name, "JNAMVAL_PRINT_NAME") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_value, "JNAMVAL_PRINT_VALUE") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_print_both, "JNAMVAL_PRINT_BOTH");

    if (!test) {
	okay = false;
    }



    /* test -t option bits */

    /* first int,float,exp */
    bits = jnamval_parse_types_option("int,float,exp");
    /* check that any number will match */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_float, "JNAMVAL_TYPE_FLOAT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_exp, "JNAMVAL_TYPE_EXP");
    if (!test) {
	okay = false;
    }

    /* just exponents */
    bits = jnamval_parse_types_option("exp");
    /* check that int and float will fail but exp will succeed */
    test = jnamval_test_bits(false, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_float, "JNAMVAL_TYPE_FLOAT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_exp, "JNAMVAL_TYPE_EXP");
    if (!test) {
	okay = false;
    }

    /* test all types */
    bits = jnamval_parse_types_option("any");
    /* verify that it is the any bit */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_any, "JNAMVAL_TYPE_ANY");
    if (!test) {
	okay = false;
    }

    /* test compound */
    bits = jnamval_parse_types_option("compound");
    /* verify that the compound type is set by compound match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_compound, "JNAMVAL_TYPE_COMPOUND");
    if (!test) {
	okay = false;
    }
    /* verify that the compound type is set by matching all types */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_object, "JNAMVAL_TYPE_OBJECT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_array, "JNAMVAL_TYPE_ARRAY");
    if (!test) {
	okay = false;
    }

    /* test member */
    bits = jnamval_parse_types_option("member");
    /* verify that the member type is set by member match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_member, "JNAMVAL_TYPE_MEMBER");
    if (!test) {
	okay = false;
    }

    /* test simple */
    bits = jnamval_parse_types_option("simple");
    /* verify that the simple type is set by simple match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_simple, "JNAMVAL_TYPE_SIMPLE");
    if (!test) {
	okay = false;
    }
    /* verify that the simple type is set by matching each type */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_num, "JNAMVAL_TYPE_NUM") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_bool, "JNAMVAL_TYPE_BOOL") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_string, "JNAMVAL_TYPE_STR") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_null, "JNAMVAL_TYPE_NULL");
    if (!test) {
	okay = false;
    }

    /* test int */
    bits = jnamval_parse_types_option("int");
    /* verify that the int type is set by int match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT");
    if (!test) {
	okay = false;
    }

    /* test float */
    bits = jnamval_parse_types_option("float");
    /* verify that the float type is set by float match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_float, "JNAMVAL_TYPE_FLOAT");
    if (!test) {
	okay = false;
    }

    /* test exp */
    bits = jnamval_parse_types_option("exp");
    /* verify that the exp type is set by exp match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_exp, "JNAMVAL_TYPE_EXP");
    if (!test) {
	okay = false;
    }

    /* test bool */
    bits = jnamval_parse_types_option("bool");
    /* verify that the bool type is set by bool match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_bool, "JNAMVAL_TYPE_BOOL");
    if (!test) {
	okay = false;
    }

    /* test string */
    bits = jnamval_parse_types_option("str");
    /* verify that the string type is set by string match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_string, "JNAMVAL_TYPE_STR");
    if (!test) {
	okay = false;
    }

    /* test null */
    bits = jnamval_parse_types_option("null");
    /* verify that the null type is set by null match function */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_null, "JNAMVAL_TYPE_NULL");
    if (!test) {
	okay = false;
    }

    /* test int,str,null */
    bits = jnamval_parse_types_option("int,str,null");
    /* verify that the int,str,null types are set by match functions */
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_string, "JNAMVAL_TYPE_STR") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_null, "JNAMVAL_TYPE_NULL");
    if (!test) {
	okay = false;
    }

    /*
     * test that none of the bits are set not via the match none function but by
     * each match function
     */
    bits = JNAMVAL_TYPE_NONE;
    test = jnamval_test_bits(false, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_float, "JNAMVAL_TYPE_FLOAT") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_exp, "JNAMVAL_TYPE_EXP") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_num, "JNAMVAL_TYPE_NUM") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_bool, "JNAMVAL_TYPE_BOOL") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_string, "JNAMVAL_TYPE_STR") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_null, "JNAMVAL_TYPE_NULL") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_object, "JNAMVAL_TYPE_OBJECT") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_array, "JNAMVAL_TYPE_ARRAY") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_any, "JNAMVAL_TYPE_ANY") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_simple, "JNAMVAL_TYPE_SIMPLE") &&
	   jnamval_test_bits(false, bits, __LINE__, jnamval_match_compound, "JNAMVAL_TYPE_COMPOUND");
    if (!test) {
	okay = false;
    }

    /* check all types */
    bits = jnamval_parse_types_option("int,float,exp,num,bool,str,null,object,array,any,simple,compound");
    test = jnamval_test_bits(true, bits, __LINE__, jnamval_match_int, "JNAMVAL_TYPE_INT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_float, "JNAMVAL_TYPE_FLOAT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_exp, "JNAMVAL_TYPE_EXP") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_num, "JNAMVAL_TYPE_NUM") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_bool, "JNAMVAL_TYPE_BOOL") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_string, "JNAMVAL_TYPE_STR") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_null, "JNAMVAL_TYPE_NULL") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_object, "JNAMVAL_TYPE_OBJECT") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_array, "JNAMVAL_TYPE_ARRAY") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_any, "JNAMVAL_TYPE_ANY") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_simple, "JNAMVAL_TYPE_SIMPLE") &&
	   jnamval_test_bits(true, bits, __LINE__, jnamval_match_compound, "JNAMVAL_TYPE_COMPOUND");
    if (!test) {
	okay = false;
    }



    return okay;
}

/* jnamval_test_number_range_opts
 *
 * Test that the number range functionality works okay.
 *
 * given:
 *
 *	option	     	option that this is testing
 *	expected     	whether test should return true or false
 *	number		number to test
 *	total_matches	number of total matches
 *	line		line number of code which called this function
 *	range		range to verify number against
 *
 * Returns true if test is okay.
 *
 * NOTE: this will not return on NULL pointers.
 */
bool
jnamval_test_number_range_opts(bool expected, intmax_t number, intmax_t total_matches,
	intmax_t line, struct json_util_number *range)
{
    bool test = false;	    /* result of test */

    if (range == NULL) {
	err(15, __func__, "NULL range, cannot test");
	not_reached();
    }

    test = json_util_number_in_range(number, total_matches, range);
    print("in function %s from line %jd: expects %s: ", __func__, line, expected?"success":"failure");
    if (range->exact) {
	print("expect exact match for number %jd: ", number);
    } else if (range->range.inclusive) {
	if (range->range.max < 0) {
	    /* if max is < 0 then it's up through the total_matches - max item */
	    print("expect number %jd to be >= %jd && <= (%jd - %jd): ", number, range->range.min, total_matches,
		    -range->range.max);
	} else {
	    print("expect number %jd to be >= %jd && <= %jd: ", number, range->range.min, range->range.max);
	}
    } else if (range->range.greater_than_equal) {
	print("expect number %jd to be >= %ju: ", number, range->range.min);
    } else if (range->range.less_than_equal) {
	print("expect number %jd to be <= %jd: ", number, range->range.max);
    }

    print("test %s\n", expected == test?"OK":"failed");

    return expected == test;
}

/* jnamval_test_bits    -	test bits code
 *
 * given:
 *
 *	expected	- whether test should fail or not
 *	set_bits	- the bits actually set
 *	line		- line number of code that called this function
 *	check_func	- pointer to function of appropriate check
 *	name		- name of bits to check
 *
 * Returns true if the test succeeds otherwise false.
 *
 * NOTE: this function will not return on NULL function pointer or NULL name.
 */
bool
jnamval_test_bits(bool expected, uintmax_t set_bits, intmax_t line, bool (*check_func)(uintmax_t), const char *name)
{
    bool okay = true;	/* assume test will pass */
    bool test = false;	/* return value of function call */

    /* firewall */
    if (check_func == NULL) {
	err(16, __func__, "NULL check_func");
	not_reached();
    } else if (name == NULL) {
	err(17, __func__, "NULL name");
	not_reached();
    }

    print("in function %s from line %jd (bits %ju): expects %s %s set: ", __func__, line, set_bits,
	    name, expected?"to be":"to NOT be");
    test = check_func(set_bits);

    if (test != expected) {
	okay = false;
    }
    print("test %s\n", okay?"OK":"failed");

    return okay;
}
