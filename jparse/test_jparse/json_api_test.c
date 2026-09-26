/*
 * json_api_test - focused API tests for json_parse helpers
 *
 * Copyright (c) 2026 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../jparse.h"
#include "../json_util.h"


static void
free_tree(struct json *node)
{
    if (node != NULL) {
	json_tree_free(node, JSON_INFINITE_DEPTH);
	free(node);
    }
}


static int
test_malformed_unicode(void)
{
    char const malformed[] = "\\u12xz";
    char *decoded = NULL;
    size_t retlen = 123;

    decoded = json_decode(malformed, strlen(malformed), false, &retlen);
    if (decoded != NULL) {
	fprintf(stderr, "json_decode() unexpectedly decoded malformed unicode escape\n");
	free(decoded);
	return 1;
    }
    if (retlen != 0) {
	fprintf(stderr, "json_decode() did not clear retlen on malformed unicode escape\n");
	return 1;
    }
    return 0;
}


static int
test_bool_slice(void)
{
    char const bool_buf[] = { 't', 'r', 'u', 'e', 'x' };
    struct json *node = NULL;

    node = json_conv_bool(bool_buf, 4);
    if (node == NULL || node->type != JTYPE_BOOL || node->item.boolean.converted == false ||
	node->item.boolean.parsed == false || node->item.boolean.value == false ||
	node->item.boolean.as_str == NULL || strcmp(node->item.boolean.as_str, "true") != 0) {
	fprintf(stderr, "json_conv_bool() failed to parse an exact-length true slice\n");
	free_tree(node);
	return 1;
    }
    free_tree(node);
    return 0;
}


static int
test_null_slice(void)
{
    char const null_buf[] = { 'n', 'u', 'l', 'l', 'x' };
    struct json *node = NULL;

    node = json_conv_null(null_buf, 4);
    if (node == NULL || node->type != JTYPE_NULL || node->item.null.converted == false ||
	node->item.null.parsed == false || node->item.null.as_str == NULL ||
	strcmp(node->item.null.as_str, "null") != 0) {
	fprintf(stderr, "json_conv_null() failed to parse an exact-length null slice\n");
	free_tree(node);
	return 1;
    }
    free_tree(node);
    return 0;
}


static int
test_parse_json_len_limit(void)
{
    struct json *tree = NULL;
    bool is_valid = true;

    tree = parse_json("{}", (size_t)INT_MAX + 1U, NULL, &is_valid);
    if (tree == NULL || is_valid != false || tree->type != JTYPE_UNSET) {
	fprintf(stderr, "parse_json() failed to reject len > INT_MAX safely\n");
	free_tree(tree);
	return 1;
    }
    free_tree(tree);
    return 0;
}


int
main(void)
{
    int ret = 0;

    ret |= test_malformed_unicode();
    ret |= test_bool_slice();
    ret |= test_null_slice();
    ret |= test_parse_json_len_limit();
    return ret;
}
