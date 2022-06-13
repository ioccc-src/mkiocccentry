/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jauthchk - IOCCC JSON .author.json checker and validator. Invoked by
 * mkiocccentry after the .author.json file has been created but prior to
 * forming the tarball. The .author.json file is not formed if the jinfochk tool
 * fails to validate the .info.json file.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This tool and jinfochk is (and are) very much a work(s) in progress and
 * as of 10 March 2022 it was decided that the parsing should be done via
 * flex(1) and bison(1) which will require some time and thought. In time the
 * two tools will be merged into one which can parse one or both of .author.json
 * and .info.json. This is because some fields MUST be the same value in both
 * files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/*
 * Our header file - #includes the header files we need
 */
#include "jauthchk.h"


/*
 * jauthchk_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths.
 *
 * given:
 *
 *      file        - path to JSON file to parse
 *      fnamchk	    - path to fnamchk util
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
jauthchk_sanity_chks(char const *file, char const *fnamchk)
{
    /*
     * firewall
     */
    if (file == NULL || fnamchk == NULL) {
	err(5, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * file must be readable
     */
    if (!exists(file)) {
	fpara(stderr,
	      "",
	      "The JSON path specified does not exist. Perhaps you made a typo?",
	      "Please check the path and try again."
	      "",
	      "    jauthchk [options] <file>"
	      "",
	      NULL);
	err(6, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!is_file(file)) {
	fpara(stderr,
	      "",
	      "The file specified, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    jauthchk [...] <file>",
	      "",
	      NULL);
	err(7, __func__, "file is not a regular file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	fpara(stderr,
	      "",
	      "The JSON path, while it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    jauthchk [...] <file>"
	      "",
	      NULL);
	err(8, __func__, "file is not readable: %s", file);
	not_reached();
    }

    /*
     * fnamchk must be executable
     */
    if (!exists(fnamchk)) {
	fpara(stderr,
	      "",
	      "We cannot find fnamchk.",
	      "",
	      "The fnamchk program performs sanity checks on the compressed tarball filename.",
	      "Perhaps you need to use:",
	      "",
	      "    jauthchk -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(9, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    jauthchk -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(10, __func__, "fnamchk is not a regular file: %s", fnamchk);
	not_reached();
    }
    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    jauthchk -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(11, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }

    /* we also check that all the tables across the IOCCC toolkit are sane */
    ioccc_sanity_chks();

    return;
}


/*
 * check_author_json  - check file as .author.json
 *
 * given:
 *
 *	file	-   path to the file to check
 *	fnamchk	-   path to our fnamchk util
 *
 * Attempts to validate the file as .author.json, reporting any problems found.
 *
 * Returns 0 if no issues were found; else it returns non-zero.
 *
 * Function does not return on error.
 */
static int
check_author_json(char const *file, char const *fnamchk)
{
    FILE *stream;
    int ret;
    int issues = 0;
    char *data;		/* .author.json contents */
    char *data_dup;	/* contents of file strdup()d */
    size_t length;	/* length of input buffer */
    char *p = NULL;	/* for field extraction */
    char *end = NULL;	/* temporary use: end of strings (p, field) for removing spaces */
    char *val = NULL;	/* current field's value being parsed */
    char *val_esc = NULL; /* for json_decode_str() on val */
    char *saveptr = NULL; /* for strtok_r() usage */
    char *array = NULL; /* array extraction */
    char *array_start = NULL; /* start of array */
    char *array_dup = NULL; /* strdup()d copy of array */
    struct json_field *author_field; /* temporary use to determine type of value of .author.json field */
    struct json_field *common_field; /* temporary use to determine type of value of common field */
    size_t loc = 0;
    bool can_be_empty = false; /* if field can be empty */
    bool is_json_string = false; /* if json field is supposed to have a string value */
    int line_num = 1; /* the 'line number' of the value */

    /*
     * firewall
     */
    if (file == NULL || fnamchk == NULL) {
	err(12, __func__, "passed NULL arg(s)");
	not_reached();
    }
    stream = fopen(file, "r");
    if (stream == NULL) {
	err(13, __func__, "couldn't open %s", file);
	not_reached();
    }

    /* read in the file */
    data = read_all(stream, &length);
    if (data == NULL) {
	err(14, __func__, "error while reading data in %s", file);
	not_reached();
    } else if (length == 0) {
	err(15, __func__, "zero length data in file %s", file);
	not_reached();
    }
    dbg(DBG_MED, "%s read length: %ju", file, (uintmax_t)length);

    /* close the stream as we no longer need it, having read in all the file */
    errno = 0;
    ret = fclose(stream);
    if (ret != 0) {
	warnp(__func__, "error in fclose to %s file %s", json_filename(AUTHOR_JSON), file);
    }
    stream = NULL;

    /*
     * duplicate data
     */
    errno = 0;
    data_dup = calloc(length+1+1, sizeof(char));
    if (data_dup == NULL) {
	errp(16, __func__, "unable to calloc %ju bytes to duplicate data",
		 (intmax_t)length+1+1);
	not_reached();
    }
    memcpy(data_dup, data, length);

    /*
     * XXX - check_(last|first)_json_char() will be removed after the JSON
     * parser is completed and this tool is updated. We still use them now as
     * removing them would necessitate some changes in the below code and right
     * now at least the files generated by mkiocccentry actually pass the test
     * (though for this tool, jauthchk, it's not complete) and we think it's
     * better to not add a temporary fix that will also be removed in time just
     * so we can remove the functions now as opposed to later.
     */

    /*
     * Verify that the very last character is a '}'. We do this check first
     * because we don't care what comes after it (in parsing) but we do care
     * what comes after the '{' (in parsing); that is to say we proceed in
     * parsing after the first '{' but after the '}' we don't continue.
     */
    if (check_last_json_char(file, data_dup, &p, '}')) {
	err(17, __func__, "last character in file %s not a '}': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_MED, "last character: '%c'", *p);

    /* remove closing brace (and any whitespace after it) */
    *p = '\0';

    /*
     * Check that the new last char is NOT a ','.
     */
    if (!check_last_json_char(file, data_dup, &p, ',')) {
	err(18, __func__, "last char is a ',' in file %s", file);
	not_reached();
    }


    /* verify that the very first character is a '{' */
    if (check_first_json_char(file, data_dup, &p, '{')) {
	err(19, __func__, "first character in file %s not a '{': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_MED, "first character: '%c'", *p);

    /* skip past the initial opening brace */
    ++p;

    /*
     * Begin to parse the file, field by field. Note that as of 1 March 2022
     * this is not complete and some files that are validly formed will trip the
     * parser up. These will be fixed in a future commit.
     */
    do {
	/* we have to skip skip leading whitespace */
	while (*p && isspace(*p))
	    ++p;

	/* if nothing is left, break out of loop */
	if (!*p) {
	    break;
	}

	/* get the next field */
	p = strtok_r(val?NULL:p, ":,", &saveptr);
	/* if NULL pointer or empty string, break out of loop */
	if (p == NULL || !*p) {
	    break;
	}

	/* skip leading whitespace on the field */
	while (*p && isspace(*p))
	    ++p;

	/* if nothing else to parse break out of the loop */
	if (!*p)
	    break;

	/* remove a single '"' if one exists at the beginning (*p == '"') */
	if (*p == '"') {
	    ++p;
	} else {
	    /* if no '"' there's a problem */
	    warn(__func__, "found no leading '\"' for field '%s' in file %s line number %d: '%c'", p, file, line_num, *p);
	    ++issues;
	}

	/* if empty string, break out of loop */
	if (!*p)
	    break;

	/* also skip trailing spaces */
	end = p + strlen(p) - 1;
	while (*end && isspace(*end))
	    *end-- = '\0';

	/* remove a single '"' if one exists at the end (*end == '"') */
	if (*end == '"') {
	    *end = '\0';
	} else {
	    /* if no trailing '"' there's also a problem */
	    warn(__func__, "found no trailing '\"' for field '%s' in file %s line number %d: '%c'", p, file, line_num, *p);
	    ++issues;
	}

	/* if string is now empty break out of loop */
	if (!*p)
	    break;

	/*
	 * After removing the spaces and a single '"' at the beginning and end,
	 * if we find a '"' in the field (name) we know it's erroneous: thus we
	 * can simply use strcmp() on it.
	 *
	 * Before we can extract the value we have to determine the field's type
	 * of value: depending on the type of value we have to handle it
	 * differently; the below function calls will help us do that.
	 *
	 * What they do is determine which field type it is: if it is common to
	 * both .info.json and .author.json or if it belongs to .author.json: if
	 * it's neither it's an error.
	 */
	author_field = find_json_field_in_table(author_json_fields, p, &loc);
	common_field = find_json_field_in_table(common_json_fields, p, &loc);

	/*
	 * If both author_field and common_field == NULL it's an invalid field
	 * which is an error regardless of test mode.
	 *
	 * XXX However we only warn because for testing if the program
	 * aborts we won't get all the tests in. When we're sure this program is
	 * complete we can make this an error.
	 *
	 * NOTE: The way we do the below checks means that we can safely have
	 * author_field == NULL && common_field == NULL: the only times we
	 * dereference it is determining if the field can be empty and if it's a
	 * json string but we do a test that it's not NULL as part of the
	 * assignment of those two bools.
	 *
	 */
	if (author_field == NULL && common_field == NULL) {
	    jwarn(JSON_CODE_RESERVED(1), __func__, file, NULL, line_num, "invalid field '%s'", p);
	    ++issues;
	}

	if (!strcmp(p, "authors")) {
	    /* TODO The below only extracts the array: it does not yet parse it.
	     * This will come in a future commit.
	     */
	    if (!author_field) {
		err(20, __func__, "authors field not found in author_json_fields table");
		not_reached();
	    }

	    /* find start of array */
	    array_start = strtok_r(NULL, ":[{", &saveptr);
	    if (array_start == NULL) {
		err(21, __func__, "unable to find beginning of array");
		not_reached();
	    }

	    /* if empty string break out of loop */
	    if (!*array_start)
		break;

	    /* extract the array */
	    array = strtok_r(NULL, "]", &saveptr);
	    if (array == NULL) {
		err(22, __func__, "unable to extract array in file %s", file);
		not_reached();
	    }

	    if (!*array) {
		err(23, __func__, "empty array in file %s", file);
		not_reached();
	    }

	    /* strdup() the array for special parsing */
	    errno = 0;
	    array_dup = strdup(array);
	    if (array_dup == NULL) {
		errp(24, __func__, "strdup() on array failed: %s", strerror(errno));
		not_reached();
	    }
	    /* Update p to go beyond the array. */
	    p = array + strlen(array) + 1;
	} else {
	    /* extract the value */
	    val = strtok_r(NULL, ",", &saveptr);
	    if (val == NULL) {
		err(25, __func__, "unable to find value in file %s for field %s", file, p);
		not_reached();
	    }

	    /* skip leading whitespace */
	    while (*val && isspace(*val))
		++val;

	    /* if empty string break out of loop */
	    if (!*val)
		break;

	    /* skip trailing whitespace */
	    end = val + strlen(val) - 1;
	    while (*end && isspace(*end))
		*end-- = '\0';

	    /* if nothing left break out of loop */
	    if (!*val)
		break;

	    can_be_empty = (common_field && common_field->can_be_empty) || (author_field && author_field->can_be_empty);
	    is_json_string = (common_field && common_field->field_type == JTYPE_STRING) ||
			     (author_field && (author_field->field_type == JTYPE_STRING));
	    /*
	     * If the field type is a string we have to remove a single '"' and
	     * from the beginning and end of the value.
	     */
	    if (is_json_string) {
		if (!strcmp(val, "\"\"")) {
		    /* make sure that it's not an empty string ("") */
		    warn(__func__, "found empty string for field '%s' in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		} else if (!strcmp(val, "null") && !can_be_empty) {
		    /* if it's null and it cannot be empty it's an issue */
		    warn(__func__, "found invalid null value for field '%s' in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		} else if (strchr(val, '"') == NULL && !can_be_empty) {
		    /* if there's no '"' and it cannot be empty it's an issue */
		    warn(__func__, "string field '%s' does not have any '\"'s in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		} else if (can_be_empty && strcmp(val, "null") && strchr(val, '"') == NULL) {
		    /* if it can be empty and it's not 'null' and there's no '"'
		     * it's an issue.
		     */
		    warn(__func__, "string field '%s' value that's not 'null' has no '\"'s in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}

		/* remove a single '"' at the beginning of the value */
		if (*val == '"') {
		    ++val;
		} else if (strcmp(val, "null")) {
		    /*
		     * If no '"' and the value is not exactly "null" (null
		     * object) it's an issue
		     */
		    warn(__func__, "found non-null string field '%s' without '\"' at the beginning in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}

		/* if nothing left continue to the next iteration of loop */
		if (!*val) {
		    warn(__func__, "found empty string value for field '%s' in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}

		/* also remove a trailing '"' at the end of the value. */
		end = val + strlen(val) - 1;
		if (*end == '"') {
		    *end = '\0';
		} else if (strcmp(val, "null")) {
		    /*
		     * if there's no trailing '"' and it's not a null object
		     * ("null") then it's an issue
		     */
		    warn(__func__, "found non-null string field '%s' without '\"' at the end in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}

		/*
		 * if nothing left it's an issue: continue to next iteration
		 * of the loop
		 */
		if (!*val) {
		    warn(__func__, "found empty string value for field '%s' in file %s", p, file);
		    ++issues;
		    continue;
		}

		/*
		 * after removing the spaces and a single '"' at the beginning and end,
		 * if we find an unescaped '"' in the field we know it's
		 * erroneous: the json decode function will flag this as an
		 * issue and an error will be issued.
		 */
	    } else {
		/*
		 * if we get here then we have to make sure there aren't any
		 * '"'s.
		 */
		if (strchr(val, '"') != NULL) {
		    warn(__func__, "found '\"' in non-string field '%s' in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}
	    }

	    /*
	     * We have to determine if characters were properly escaped
	     * according to the JSON spec.
	     */
	    val_esc = json_decode_str(val, NULL);
	    if (val_esc == NULL) {
		err(26, __func__, "json_decode(): invalidly formed field '%s' value '%s' or malloc failure in file %s",
			p, val, file);
		not_reached();
	    }

	    /* if empty value, free value and break out of loop */
	    if (!*val_esc) {
		free(val_esc);
		val_esc = NULL;
		break;
	    }

	    /* handle regular field */
	    if (add_common_json_field(file, p, val_esc, line_num)) {
	    } else if (add_author_json_field(file, p, val_esc, line_num)) {
	    } else {
		/* this should actually never be reached */
		warn(__func__, "invalid field found in file %s: '%s'", file, p);
		++issues;
	    }

	    /* free the JSON decoded value */
	    free(val_esc);
	    val_esc = NULL;

	    ++line_num;
	}
    } while (true);

    /* free data */
    if (data != NULL) {
	/* Yes it's safe to free() a NULL pointer but we check for NULL anyway. */
	free(data);
	data = NULL;
    }

    /* free strdup()d data */
    if (data_dup != NULL) {
	/* Yes it's safe to free() a NULL pointer but we check for NULL anyway. */
	free(data_dup);
	data_dup = NULL;
    }

    /* free strdup()d array */
    if (array_dup != NULL) {
	/* Yes it's safe to free() a NULL pointer but we check for NULL anyway. */
	free(array_dup);
	array_dup = NULL;
    }

    /* check the found_author_json_fields list for issues */
    issues += check_found_author_json_fields(file, test);

    /* now free the found_author_json_fields list.
     *
     * NOTE: after this the list will be NULL.
     */
    free_found_author_json_fields();

    /*
     * check found_common_json_fields list, updating the number of issues found
     */
    issues += check_found_common_json_fields(file, fnamchk, test);

    /* free the found_common_json_fields list.
     *
     * NOTE: After this the list will be NULL.
     */
    free_found_common_json_fields();

    /* if issues != 0 there will be a non-zero return status of jauthchk */
    return issues;
}


/*
 * add_author_json_field - check field name
 *
 * Check if name is a .author.json field and if it is add it to the
 * found_author_json_fields list.
 *
 * given:
 *
 *	json_filename	- the file being parsed (path to)
 *	name		- the field name
 *	val		- the value of the field
 *	line_num	- the 'line number' of the value
 *
 * returns:
 *	true ==> if the name is a .author.json field and it's added to the list
 *	false ==> if it's not one of the .author.json fields
 *
 * NOTE: Does not return on error (NULL pointers).
 *
 * XXX The function name is misleading. It would seem to me by name that it is
 * supposed to add it to the table author_json_fields but it does not. This
 * function is called in order to check if the field is a .author.json field and
 * if so add it to the found_author_json_fields table. This function actually
 * uses add_found_author_json_field() but perhaps that function should do all of
 * the work. This should be looked into and probably fixed but it will be done
 * after the json parser is complete if it's decided that this function is still
 * useful (which I think it will be).
 */
bool
add_author_json_field(char const *json_filename, char *name, char *val, int line_num)
{
    bool ret = true;	/* return value: true ==> known field and added, false ==> not a common field */
    struct json_field *field = NULL; /* the field in the author_json_fields table if found */
    size_t loc = 0; /* location in the author_json_fields table */

    /*
     * firewall
     */
    if (json_filename == NULL || name == NULL || val == NULL) {
	err(27, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * check if the name is an expected common field
     */
    field = find_json_field_in_table(author_json_fields, name, &loc);
    if (field != NULL) {
	dbg(DBG_MED, "found field '%s' with value '%s'", field->name, val);
	add_found_author_json_field(json_filename, field->name, val, line_num);
    } else {
	ret = false;
    }
    return ret;
}


/*
 * check_found_author_json_fields - found_author_json_fields table value checks
 *
 * Verify that all the fields in the found_author_json_fields table have
 * values that are valid and that all fields that are required are in the file
 * (TODO as of 3 March 2022 this is not actually done because arrays are not yet
 * parsed: we have to extract all fields first before this test can be added).
 *
 *  given:
 *
 *	    json_filename   - .author.json file we're checking
 *	    test	    - if test mode: ignore some checks
 *
 *
 * returns:
 *	>0 ==> the number of issues found
 *	0 ==> if no issues were found
 */
int
check_found_author_json_fields(char const *json_filename, bool test)
{
    struct json_field *field; /* current field in found_author_json_fields list */
    struct json_value *value; /* current value in current field's values list */
    struct json_field *author_field = NULL; /* element in the author_json_fields table */
    size_t loc = 0;	/* location in the author_json_fields table */
    int issues = 0;
    size_t val_length = 0;  /* current value length */
    int author_count = 0;

    /*
     * firewall
     */
    if (json_filename == NULL) {
	err(28, __func__, "passed NULL json_filename");
	not_reached();
    }

    for (field = found_author_json_fields; field != NULL; field = field->next) {
	/*
	 * first make sure the name != NULL and strlen() > 0
	 */
	if (field->name == NULL || strlen(field->name) <= 0) {
	    jerr(JSON_CODE_RESERVED(9), __func__, __FILE__, NULL, __LINE__,
					"found NULL or empty field in found_author_json_fields list in file %s",
					json_filename);
	    not_reached();
	}

	/* now make sure it's allowed to be in this table. */
	loc = 0;
	author_field = find_json_field_in_table(author_json_fields, field->name, &loc);

	/*
	 * If the field is not allowed in the list then it suggests there is a
	 * problem in the code because only author fields should be added to the
	 * list in the first place. Thus it's an error if a field that's in the
	 * author list is not an author field name.
	 */
	if (author_field == NULL) {
	    jerr(JSON_CODE_RESERVED(10), __func__, __FILE__, NULL, __LINE__,
					 "illegal field name '%s' in found_author_json_fields list in file %s",
					 field->name, json_filename);
	    not_reached();
	}

	dbg(DBG_VHIGH, "checking field '%s' in file %s", field->name, json_filename);
	for (value = field->values; value != NULL; value = value->next) {
	    char *val = value->value;

	    if (val == NULL) {
		jerr(JSON_CODE_RESERVED(11), __func__, __FILE__, NULL, __LINE__,
					     "NULL pointer val for field '%s' in file %s",
					     field->name, json_filename);
		not_reached();
	    }

	    val_length = strlen(val);

	    if (val_length <= 0) {
		warn(__func__, "empty value found for field '%s' in file %s: '%s'", field->name, json_filename, val);
		/*
		 * NOTE: don't increase issues because the below checks will do
		 * that too: this warning only notes the reason the test will
		 * fail.
		 */
	    }

	    /* make sure the field is not over the limit allowed */
	    if (author_field->max_count > 0 && author_field->count > author_field->max_count) {
		jwarn(JSON_CODE(1), __func__, __FILE__, NULL, value->line_num,
				    "field '%s' found %ju times but is only allowed %ju time%s in file %s",
				    author_field->name, (uintmax_t)author_field->count,
				    (uintmax_t)author_field->max_count,
				    author_field->max_count==1?"":"s", json_filename);
		++issues;
	    }


	    /*
	     * Now we have to do checks on the field type. We only have to check
	     * numbers and bools: because for strings there's nothing to
	     * check: we remove the outermost '"'s and then use strcmp() whereas
	     * for numbers and bools we want to make sure that they're actually
	     * valid values.
	     */
	    switch (author_field->field_type) {
		case JTYPE_BOOL:
		    if (strcmp(val, "false") && strcmp(val, "true")) {
			warn(__func__, "bool field '%s' has non boolean value in file %s: '%s'",
				       author_field->name, json_filename, val);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "%s is a bool", val);
		    }
		    break;
		case JTYPE_NUMBER:
		    if (!is_decimal(val, val_length)) {
			warn(__func__, "number field '%s' has non-number value in file %s: '%s'",
				       author_field->name, json_filename, val);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "%s is an integer", val);
		    }
		    break;
		default:
		    break;
	    }

	    if (!strcmp(field->name, "IOCCC_author_version")) {
		if (!test && strcmp(val, AUTHOR_VERSION)) {
		    warn(__func__, "IOCCC_author_version != \"%s\" in file %s: \"%s\"", AUTHOR_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "jauthchk_version")) {
		if (!test && strcmp(val, JAUTHCHK_VERSION)) {
		    warn(__func__, "jauthchk_version != JAUTHCHK_VERSION \"%s\" in file %s: \"%s\"",
				   JAUTHCHK_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "author_count")) {
		author_count = string_to_int(val);
		if (!(author_count > 0 && author_count <= MAX_AUTHORS)) {
		    warn(__func__, "author count out of range of > 1 && <= %d in file %s: '%s' (%d)",
				   MAX_AUTHORS, json_filename, val, author_count);
		    ++issues;
		}
	    } else {
		/*
		 * This should never actually be reached but if it is it
		 * suggests that a field was not added to be checked: if it's
		 * not a valid field we would have already detected and aborted
		 * earlier in this loop so we don't have to check for that.
		 */
		warn(__func__, "found unhandled author field in file %s: '%s'", json_filename, field->name);
		/*
		 * NOTE: Don't increment issues because this doesn't mean
		 * there's anything wrong with the .author.json file but rather
		 * that the field isn't verified.
		 *
		 * XXX On the other hand it can result in a false positive of a
		 * valid test so this has to be carefully considered. However
		 * since the jinfochk and jauthchk tools will change
		 * dramatically this function might not even exist (or if it
		 * does it'll be very different). Either the idea that an
		 * unhandled field is not an issue with the file could be
		 * reassessed even if an unhandled field is actually a problem
		 * with the tools themselves. This same logic applies to the
		 * .info.json and common fields check functions.
		 */
	    }
	}
    }

    return issues;
}

/*
 * add_found_author_json_field - add field:value to found_author_json_fields list
 *
 * Add a field:value pair to to the found_author_json_fields list.
 *
 * given:
 *
 *	name			- field name
 *	val			- field value
 *	line_num		- the 'line number' of the value
 *	filename		- filename being parsed
 *
 * Returns the newly allocated struct json_field * added to the
 * found_author_json_fields list.
 *
 * NOTE: If the field is already in the list we just add the value to the values
 * list in the struct json_value * within the struct json_field *. Does not
 * return on error.
 *
 */
static struct json_field *
add_found_author_json_field(char const *json_filename, char const *name, char const *val, int line_num)
{
    struct json_field *field = NULL; /* iterate through fields list to find the field (or if not found, create a new field) */
    struct json_value *value = NULL; /* the new value */
    struct json_field *field_in_table = NULL;
    size_t loc = 0; /* location in author_json_fields table */

    /*
     * firewall
     */
    if (name == NULL || val == NULL || json_filename == NULL) {
	jerr(JSON_CODE_RESERVED(6), __func__, __FILE__, NULL, __LINE__, "passed NULL arg(s)");
	not_reached();
    }

    field_in_table = find_json_field_in_table(author_json_fields, name, &loc);
    if (field_in_table == NULL) {
	err(29, __func__, "called add_found_author_json_field() on field '%s' not specific "
			  "to .author.json in file %s", name, json_filename);
	not_reached();
    }
    /*
     * Set in table that it's found and increment the number of times it's been
     * seen.
     */
    author_json_fields[loc].count++;
    author_json_fields[loc].found = true;

    for (field = found_author_json_fields; field != NULL; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    /*
	     * we found a field already in the list, add the value (even if this
	     * value was already in the list as this is needed in some cases).
	     */
	    value = add_json_value(json_filename, field, val, line_num);
	    if (value == NULL) {
		/*
		 * this shouldn't happen as if add_json_value() gets an error
		 * it'll abort but just to be safe we check here too
		 */
		jerr(JSON_CODE_RESERVED(7), __func__, __FILE__, NULL, __LINE__,
					    "couldn't add value '%s' to field '%s' file %s",
					    val, field->name, json_filename);
		not_reached();
	    }

	    field->count++; /* update count */

	    return field; /* already in the list: just return it after incrementing the count and adding the new value */
	}
    }

    /*
     * okay we got here which means we have to create a new field in the list
     * with the value passed in
     */
    field = new_json_field(json_filename, name, val, line_num);
    if (field == NULL) {
	/*
	 * we should never get here because if new_json_field gets NULL it
	 * aborts the program.
	 */
	jerr(JSON_CODE_RESERVED(8), __func__, __FILE__, NULL, __LINE__,
				    "error creating new struct json_field * for field '%s' value '%s' file %s",
				    name, val, json_filename);
	not_reached();
    }

    /* add to the list */
    field->next = found_author_json_fields;
    found_author_json_fields = field;

    dbg(DBG_VHIGH, "added field '%s' value '%s'", field->name, val);

    return field;
}

/*
 * free_found_author_json_fields - free the authors json fields list
 *
 * This function returns void.
 *
 */
static void
free_found_author_json_fields(void)
{
    struct json_field *field, *next_field = NULL;

    for (field = found_author_json_fields; field != NULL; field = next_field) {
	next_field = field->next;
	free_json_field(field);
	field = NULL;
    }

    found_author_json_fields = NULL;
}

/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: '%s'", program);
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	program		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *str, char const *prog)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FNAMCHK_PATH_0, JAUTHCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}

int
main(int argc, char **argv)
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *file;				/* file argument to check */
    int ret;				/* libc return code */
    int issues;				/* issues found */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false;	/* true ==> -F fnamchk used */
    int code;				/* a JSON error code */
    int i;				/* return value of getopt() */

    /*
     * parse args
     */
    program = argv[0];
    program_basename = base_name(program);
    while ((i = getopt(argc, argv, "hv:VqnF:tW:w")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(1, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", JAUTHCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JAUTHCHK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'F':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't':
	    test = true;
	    break;
	case 'W':
	    /* parse ignore code
	     *
	     * NOTE: Although the JSON warn codes 0 - 999 are 0-padded the warn
	     * codes are _NOT_ in octal (0 notwithstanding but 0 is reserved)
	     * and we explicitly parse them as decimal!
	     */
	    errno = 0;
	    code = (int)strtol(optarg, NULL, 10);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __func__, "cannot parse -W arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
		not_reached();
	    }
	    /* add code to ignore_json_code_set[] */
	    ignore_json_code(code);
	    break;
	case 'w':
	    show_full_json_warnings = true;
	    break;
	default:
	    usage(1, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(1, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }
    file = argv[optind];
    dbg(DBG_LOW, "JSON file: %s", file);

    /*
     * Welcome
     */
    if (!quiet) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = printf("Welcome to jauthchk version: %s\n", JAUTHCHK_VERSION);
	if (ret <= 0) {
	    errp(30, __func__, "printf error printing the welcome string");
	    not_reached();
	}

	/*
	 * environment sanity checks
	 */
	para("", "Performing sanity checks on your environment ...", NULL);
    }

    /*
     * validate JSON encoding table
     */
    jencchk();
    if (!quiet) {
	para("", "... JSON encoding table looks OK ...", NULL);
    }

    /* we have to find the fnamchk util */
    find_utils(false, NULL, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk,
	       false, NULL, false, NULL);

    jauthchk_sanity_chks(file, fnamchk);
    if (!quiet) {
	para("... environment looks OK", "", NULL);
    }

    issues = check_author_json(file, fnamchk);

    if (program_basename != NULL) {
	free(program_basename);
	program_basename = NULL;
    }

    /*
     * XXX - TODO Once the authors array is parsed we have to call
     * free_author_array() (maybe: it will depend on how it's done).
     */

    if (issues != 0 && !test) {
	dbg(DBG_LOW, "%s is invalid", file);
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(issues != 0); /*ooo*/
}
