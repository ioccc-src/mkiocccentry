/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jinfochk - IOCCC JSON .info.json checker and validator. Invoked by
 * mkiocccentry after the .info.json file has been created but prior to forming
 * the .author.json file, validating it with jauthchk and then forming the
 * tarball.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This tool and jauthchk is (and are) very much a work(s) in progress and
 * as of 10 March 2022 it was decided that the parsing should be done via
 * flex(1) and bison(1) which will require some time and thought. In time the
 * two tools will be merged into one which can parse one or both .info.json and
 * .author.json. This is because some fields MUST be the same value in both
 * files.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>	/* for strcasecmp() */
#include <ctype.h>

/*
 * Our header file - #includes the header files we need
 */
#include "jinfochk.h"

/*
 * jinfochk_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths and tables.
 *
 * given:
 *
 *      file        - path to JSON file to parse
 *      fnamchk	    - path to fnamchk util
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
jinfochk_sanity_chks(char const *file, char const *fnamchk)
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
	      "    jinfochk [options] <file>"
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
	      "    jinfochk [...] <file>",
	      "",
	      NULL);
	err(7, __func__, "file is not a file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	fpara(stderr,
	      "",
	      "The JSON path, while it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    jinfochk [...] <file>"
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
	      "The fnamchk program performs a sanity check on the compressed tarball filename.",
	      "Perhaps you need to use:",
	      "",
	      "    jinfochk -F /path/to/fnamchk ...",
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
	      "The fnamchk tool, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    jinfochk -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(10, __func__, "fnamchk is not a file: %s", fnamchk);
	not_reached();
    }
    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    jinfochk -F /path/to/fnamchk ...",
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
 * check_info_json  - check file as .info.json
 *
 * given:
 *
 *	file	-   path to the file to check
 *	fnamchk -   path to our fnamchk util
 *
 * Attempts to validate the file as .info.json, reporting any problems found.
 *
 * Returns 0 if no issues were found; else it returns non-zero.
 *
 * Function does not return on error.
 */
static int
check_info_json(char const *file, char const *fnamchk)
{
    FILE *stream;
    int ret;
    int issues = 0;
    char *data;		/* .info.json contents */
    char *data_dup;	/* contents of file strdup()d */
    size_t length;	/* length of input buffer */
    char *p = NULL;	/* for field extraction */
    char *end = NULL;	/* temporary use: end of strings (p, field) for removing spaces */
    char *val = NULL;	/* current field's value being parsed */
    char *val_esc = NULL; /* for malloc_json_decode_str() */
    char *saveptr = NULL; /* for strtok_r() usage */
    char *array = NULL; /* array extraction */
    char *array_start = NULL; /* start of array */
    char *array_dup = NULL; /* strdup()d copy of array */
    char *array_field = NULL; /* for extraction of array fields */
    char *array_saveptr = NULL; /* for strtok_r() on array_dup */
    char *array_val = NULL; /* for extraction of array values */
    char *array_val_esc = NULL; /* for malloc_json_decode_str() */
    struct json_field *info_field; /* temporary use to determine type of value of .info.json field */
    struct json_field *array_info_field; /* temporary use to determine type of value of array .info.json field */
    struct json_field *common_field; /* temporary use to determine type of value if common field */
    size_t loc = 0;
    bool can_be_empty = false; /* if field can be empty */
    bool is_json_string = false; /* if field is a json string type */
    int line_num = 1; /* actually field number */

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
    dbg(DBG_HIGH, "%s read length: %ju", file, (uintmax_t)length);

    /* close the stream as we no longer need it, having read in all the file */
    errno = 0;
    ret = fclose(stream);
    if (ret != 0) {
	warnp(__func__, "error in fclose to %s file %s", json_filename(INFO_JSON), file);
    }
    stream = NULL;

    /* scan for embedded NUL bytes (before EOF) */
    if (!is_string(data, length+1)) {
	err(16, __func__, "found NUL before EOF: %s", file);
	not_reached();
    }

    errno = 0;
    data_dup = strdup(data);
    if (data_dup == NULL) {
	errp(17, __func__, "unable to strdup file %s contents", file);
	not_reached();
    }

    /*
     * Verify that the very last character is a '}'. We do this check first
     * because we don't care what comes after it (in parsing) but we do care
     * what comes after the '{' (in parsing); that is to say we proceed in
     * parsing after the first '{' but after the '}' we don't continue.
     */
    if (check_last_json_char(file, data_dup, strict, &p, '}')) {
	err(18, __func__, "last character in file %s not a '}': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_HIGH, "last character: '%c'", *p);

    /* remove closing brace (and any whitespace after it) */
    *p = '\0';

    /*
     * Check that the new last char is NOT a ','. Don't do strict checking
     * because we want the end spaces to be trimmed off first.
     */
    if (!check_last_json_char(file, data_dup, false, &p, ',')) {
	err(19, __func__, "last char is a ',' in file %s", file);
	not_reached();
    }

    /* verify that the very first character is a '{' */
    if (check_first_json_char(file, data_dup, strict, &p, '{')) {
	err(20, __func__, "first character in file %s not a '{': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_HIGH, "first character: '%c'", *p);

    /* skip past the initial opening brace */
    ++p;

    /*
     * Begin to parse the file, field by field.
     */
    do {
	/* we have to skip leading whitespace */
	while (*p && isspace(*p))
	    ++p;

	/* get the next field */
	p = strtok_r(val?NULL:p, ":,", &saveptr);
	/* if NULL pointer or empty string, break out of loop */
	if (p == NULL || *p == '\0') {
	    break;
	}

	/* skip leading whitespace on the field */
	while (*p != '\0' && isspace(*p))
	    ++p;

	/* if nothing else to parse break out of the loop */
	if (*p == '\0')
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
	    continue;

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
	 * both .info.json and .author.json or if it belongs to .info.json: if
	 * it's neither it's an error.
	 */
	info_field = find_json_field_in_table(info_json_fields, p, &loc);
	common_field = find_json_field_in_table(common_json_fields, p, &loc);

	/*
	 * If both info_field and common_field == NULL it's an invalid field
	 * which is an error regardless of test mode.
	 *
	 * XXX However we only warn because for testing if the program
	 * aborts we won't get all the tests in. When we're sure this program is
	 * complete we can make this an error.
	 *
	 * NOTE: The way we do the below checks means that we can safely have
	 * info_field == NULL && common_field == NULL: the only times we
	 * dereference it is determining if the field can be empty and if it's a
	 * json string but we do a test that it's not NULL as part of the
	 * assignment of those two bools.
	 *
	 */
	if (info_field == NULL && common_field == NULL) {
	    jwarn(JSON_CODE_RESERVED(1), program, __func__, file, NULL, line_num, "invalid field '%s'", p);
	    ++issues;
	}

	/* manifest array is handled specially */
	if (!strcmp(p, "manifest")) {
	    if (!info_field) {
		err(21, __func__, "manifest field not found in info_json_fields table");
		not_reached();
	    }


	    /*
	     * Add the manifest field to the found_info_json_fields list: it can
	     * have an empty value as all we care about is that it's been seen;
	     * the values of the array we will parse below.
	     */
	    if (!add_info_json_field(file, "manifest", "", line_num)) {
		err(22, __func__, "couldn't add manifest to found_info_json_fields");
		not_reached();
	    }

	    ++line_num;

	    /* find start of array */
	    array_start = strtok_r(NULL, "{", &saveptr);
	    if (array_start == NULL) {
		err(23, __func__, "unable to find beginning of array");
		not_reached();
	    }

	    /* if empty string break out of loop */
	    if (!*array_start)
		break;


	    /* extract the array */
	    array = strtok_r(NULL, "]", &saveptr);
	    if (array == NULL) {
		err(24, __func__, "unable to extract array in file %s", file);
		not_reached();
	    }

	    /* the array cannot be empty */
	    if (*array == '\0') {
		err(25, __func__, "empty array in file %s", file);
		not_reached();
	    }

	    /* strdup() the array for special parsing */
	    errno = 0;
	    array_dup = strdup(array);
	    if (array_dup == NULL) {
		errp(26, __func__, "strdup() on array failed: %s", strerror(errno));
		not_reached();
	    }

	    /* the last array element cannot end with a ',' */
	    if (!check_last_json_char(file, array_dup, false, &p, ',')) {
		warn(__func__, "last array element ends with ',' in file %s: '%c'", file, *p);
		++issues;
	    }


	    do {
		array_field = strtok_r(array_val?NULL:array_dup, "{ :\t\n},", &array_saveptr);
		if (array_field == NULL) {
		    /* not necessarily an error */
		    break;
		}
		/* skip leading whitespace on the field */
		while (*array_field && isspace(*array_field))
		    ++array_field;


		/* remove a single '"' if one exists at the beginning
		 * (*array_field == '"') */
		if (*array_field == '"') {
		    ++array_field;
		} else {
		    /* if no '"' there's a problem */
		    warn(__func__, "found no leading '\"' in array field in file %s: '%s'", file, array_field);
		    ++issues;
		    break;
		}

		/* if nothing left break out of loop */
		if (!*array_field)
		    break;

		/* also skip trailing spaces */
		end = array_field + strlen(array_field) - 1;
		while (*end && isspace(*end))
		    *end-- = '\0';

		/* remove a single '"' if one exists at the end (*end == '"') */
		if (*end == '"') {
		    *end = '\0';
		} else {
		    /* if there's no trailing '"' there's also a problem */
		    warn(__func__, "found no trailing '\"' in array field in file %s: '%s'", file, array_field);
		    ++issues;
		    break;
		}

		/* if nothing left break out of loop */
		if (!*array_field) {
		    err(27, __func__, "found empty array field in file %s: '%s'", file, array_field);
		    not_reached();
		}

		array_info_field = find_json_field_in_table(info_json_fields, array_field, &loc);
		/*
		 * If array_info_field == NULL it's an invalid field which is an
		 * error regardless of test mode.
		 *
		 * XXX However we only warn because for testing if the program
		 * aborts we won't get all the tests in. When we're sure this
		 * program is complete we can make this an error.
		 *
		 * NOTE: The way we do the below checks means that we can safely
		 * have array_info_field == NULL: the only times we dereference it
		 * is determining if the field can be empty and if it's a json
		 * string but we do a test that it's not NULL as part of the
		 * assignment of those two bools.
		 */
		if (array_info_field == NULL) {
		    warn(__func__, "invalid field '%s' in manifest array of file %s", array_field, file);
		    ++issues;
		}

		/* only some array fields can have empty values (null string) */
		can_be_empty = array_info_field && array_info_field->can_be_empty;
		is_json_string = array_info_field && array_info_field->field_type == JTYPE_STRING;


		array_val = strtok_r(NULL, ":,", &array_saveptr);
		if (array_val == NULL) {
		    err(28, __func__, "array element %s without value", array_field);
		    not_reached();
		}
		/* remove leading spaces from value */
		while (*array_val && isspace(*array_val))
		    ++array_val;

		if (!*array_val)
		    break;

		/*
		 * remove any spaces at the end of the value (prior to the
		 * closing '}'
		 */
		end = array_val + strlen(array_val) - 1;
		while (*end && isspace(*end))
		    *end-- = '\0';

		/* remove a single '}' if one exists at the end (*end == '}') */
		if (*end == '}')
		    *end-- = '\0';

		/* if empty value break out of loop */
		if (!*array_val)
		    break;

		/* if the field type is a string we have to remove outer '"'s */
		if (is_json_string) {
		    if (!strcmp(array_val, "\"\"")) {
			/* make sure that it's not an empty string ("") */
			warn(__func__, "found empty string for array field '%s': '%s'", array_field, array_val);
			++issues;
			continue;
		    } else if (!strcmp(array_val, "null") && !can_be_empty) {
			/* if it's null and it cannot be empty it's an issue */
			warn(__func__, "found invalid null value for field '%s' in file %s: '%s'", array_field, file, array_val);
			++issues;
			continue;
		    } else if (strchr(array_val, '"') == NULL && !can_be_empty) {
			/* if there's no '"' and it cannot be empty it's an issue */
			warn(__func__, "string field '%s' value does not have any '\"'s in file %s: '%s'",
				       array_field, file, array_val);
			++issues;
			continue;
		    } else if (can_be_empty && strcmp(array_val, "null") && strchr(array_val, '"') == NULL) {
			/* if it can be empty and it's not 'null' and there's no '"'
			 * it's an issue.
			 */
			warn(__func__, "string field '%s' value that's not 'null' has "
				       "no '\"'s in file %s: '%s'", array_field, file, array_val);
			++issues;
			continue;
		    }

		    /* remove a single '"' at the beginning of the value */
		    if (*array_val == '"') {
			++array_val;
		    } else if (!strcmp(array_val, "null") && !can_be_empty) {
			warn(__func__, "found invalid null value for field '%s' in file %s: '%s'", array_field, file, array_val);
			++issues;
			continue;
		    } else if (strcmp(array_val, "null")) {
			/*
			 * If no '"' and the value is not exactly "null" (null
			 * object) it's an issue
			 */
			warn(__func__, "found non-null string field '%s' without '\"' at "
				       "the beginning in file %s: '%s'", array_field, file, array_val);
			++issues;
			continue;
		    }

		    /* also remove a trailing '"' at the end of the value. */
		    end = array_val + strlen(array_val) - 1;
		    if (*end == '"') {
			*end = '\0';
		    } else if (strcmp(array_val, "null")) {
			/*
			 * if there's no trailing '"' and it's not a null object
			 * ("null") then it's an issue
			 */
			warn(__func__, "found non-null string field '%s' without '\"' at the end in file %s: '%s'",
				       array_field, file, array_val);
			++issues;
			continue;
		    }

		    /*
		     * if nothing left it's an issue: continue to next iteration
		     * of the loop
		     */
		    if (!*array_val) {
			warn(__func__, "found empty string value for array field '%s' in file %s: '%s'",
				       array_field, file, array_val);
			++issues;
			continue;
		    }

		} else {
		    /*
		     * if it's not a string and there are any quotes it's an
		     * issue. Currently all fields in the manifest array are
		     * strings but we do it this way in case in the future a
		     * non-string array is added to the array.
		     */
		    if (strchr(array_val, '"') != NULL) {
			warn(__func__, "found '\"' in non-string array field '%s' value in file %s: '%s'",
				       array_field, file, array_val);
			++issues;
			continue;
		    }
		}
		/*
		 * We have to determine if characters were properly escaped
		 * according to the JSON spec.
		 */
		array_val_esc = malloc_json_decode_str(array_val, NULL, strict);
		if (array_val_esc == NULL) {
		    err(29, __func__, "malloc_json_decode(): invalidly formed field '%s' value '%s' or malloc failure in file %s",
			    array_field, array_val, file);
		    not_reached();
		}

		/* if empty value break out of loop */
		if (!*array_val_esc) {
		    free(array_val_esc);
		    array_val_esc = NULL;
		    break;
		}

		if (!add_info_json_field(file, array_field, array_val_esc, line_num)) {
		    warn(__func__, "found invalid field in array in file %s: '%s'", file, array_field);
		    ++issues;
		}

		/* free the decoded array value */
		free(array_val_esc);
		array_val_esc = NULL;
		++line_num;

	    } while (true);
	    /* Update p to go beyond the array. */
	    p = array + strlen(array) + 1;

	    /* if nothing left break out of loop */
	    if (!*p)
		break;

	} else {
	    /* extract the value */
	    val = strtok_r(NULL, ",\0", &saveptr);
	    if (val == NULL) {
		err(30, __func__, "unable to find value in file %s for field '%s'", file, p);
		not_reached();
	    }

	    /* skip leading whitespace */
	    while (*val && isspace(*val))
		++val;

	    /* if empty value break out of loop */
	    if (!*val)
		break;

	    /* skip trailing whitespace */
	    end = val + strlen(val) - 1;
	    while (*end && isspace(*end))
		*end-- = '\0';

	    /* if nothing else break out of loop */
	    if (!*val)
		break;

	    can_be_empty = (common_field && common_field->can_be_empty) || (info_field && info_field->can_be_empty);
	    is_json_string = (common_field && common_field->field_type == JTYPE_STRING) ||
			     (info_field && info_field->field_type == JTYPE_STRING);

	    /*
	     * If the field type is a string we have to remove a single '"' from
	     * the beginning and end of the value.
	     */
	    if (is_json_string) {
		if (!strcmp(val, "\"\"")) {
		    /* make sure that it's not an empty string ("") */
		    warn(__func__, "found empty string for field '%s': '%s'", p, val);
		    ++issues;
		    continue;
		} else if (!strcmp(val, "null") && !can_be_empty) {
		    /* if it's null and it cannot be empty it's an issue */
		    warn(__func__, "found invalid null value for field '%s' in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		} else if (strchr(val, '"') == NULL && !can_be_empty) {
		    /* if there's no '"' and it cannot be empty it's an issue */
		    warn(__func__, "string field '%s' value does not have any '\"'s in file %s: '%s'", p, file, val);
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
		    warn(__func__, "found empty string field '%s' in file %s: '%s'", p, file, val);
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
		    warn(__func__, "found empty string value for field '%s' in file %s: '%s'", p, file, val);
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
		    warn(__func__, "found '\"' in non-string field '%s' value in file %s: '%s'", p, file, val);
		    ++issues;
		    continue;
		}
	    }


	    /*
	     * We have to determine if characters were properly escaped
	     * according to the JSON spec.
	     */
	    val_esc = malloc_json_decode_str(val, NULL, strict);
	    if (val_esc == NULL) {
		err(31, __func__, "malloc_json_decode_str(): invalidly formed field '%s' value '%s' or "
				  "malloc failure in file %s", p, val, file);
		not_reached();
	    }

	    /* if empty value, free value and break out of loop */
	    if (!*val_esc) {
		free(val_esc);
		val_esc = NULL;
		break;
	    }

	    /* handle regular field */
	    if (add_common_json_field(program_basename, file, p, val_esc, line_num)) {
	    } else if (add_info_json_field(file, p, val_esc, line_num)) {
	    } else {
		/* this should actually never be reached */
		warn(__func__, "invalid field '%s' found in file %s", p, file);
		++issues;
	    }

	    /* free the JSON decoded value */
	    free(val_esc);
	    val_esc = NULL;

	    ++line_num;
	}
    } while (true); /* end do while */

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

    /* check the found_info_json_fields list for issues */
    issues += check_found_info_json_fields(file, test);

    /* now free the found_info_json_fields list.
     *
     * NOTE: after this the list will be NULL.
     */
    free_found_info_json_fields();

    /*
     * check the found_common_json_fields list, updating the number of issues found
     */
    issues += check_found_common_json_fields(program_basename, file, fnamchk, test);

    /* free the found_common_json_fields list.
     *
     * NOTE: After this the list will be NULL.
     */
    free_found_common_json_fields();

    /*
     * free the manifest files list: manifest_files_list.
     *
     * NOTE: After this the list will be NULL.
     */
    free_manifest_files_list();


    /* if issues != 0 there will be a non-zero return status of jinfochk */
    return issues;
}


/*
 * add_info_json_field
 *
 * Check if a name is a known .info.json field.  If it is a known field,
 * add that name to the found_info_json list.
 *
 * given:
 *
 *	file	    - the file being parsed (path to)
 *	name	    - the field name
 *	val	    - the value of the field
 *	line_num    - if field is valid add the number to the field's value
 *
 * returns:
 *	true ==> if the name is a .info.json field and was added to the list
 *	false ==> if it's not one of the .info.json fields
 *
 * NOTE: Does not return on error (NULL pointers).
 */
bool
add_info_json_field(char const *json_filename, char *name, char *val, int line_num)
{
    bool ret = true;	/* return value: true ==> known field added to list, false ==> not a .info.json field */
    struct json_field *field = NULL; /* the field in the info_json_fields table if found */
    size_t loc = 0; /* location in the info_json_fields table */

    /*
     * firewall
     */
    if (json_filename == NULL || name == NULL || val == NULL) {
	err(32, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * check if the name is an expected common field
     */
    field = find_json_field_in_table(info_json_fields, name, &loc);
    if (field != NULL) {
	dbg(DBG_HIGH, "found field '%s' with value '%s' in file %s", name, val, json_filename);
	add_found_info_json_field(json_filename, name, val, line_num);
    } else {
	ret = false;
    }
    return ret;
}


/*
 * add_found_info_json_field - add a field:value to found_info_json_fields list
 *
 * Add a field:value pair to the to the found_info_json_fields list.
 *
 * given:
 *
 *	json_filename		- name of the file being parsed
 *	name			- field name
 *	val			- field value
 *	line_num		- the 'line number'
 *
 * Returns the newly allocated struct json_field * added to the
 * found_info_json_fields list.
 *
 * NOTE: If the field is already in the list we just add the value to the values
 * list in the struct json_value * within the struct json_field *. Does not
 * return on error.
 *
 */
static struct json_field *
add_found_info_json_field(char const *json_filename, char const *name, char const *val, int line_num)
{
    struct json_field *field = NULL; /* iterate through fields list to find the field (or if not found, create a new field) */
    struct json_value *value = NULL; /* the new value */
    struct json_field *field_in_table = NULL;
    size_t loc = 0; /* location in info_json_fields table */

    /*
     * firewall
     */
    if (name == NULL || val == NULL || json_filename == NULL) {
	jerr(JSON_CODE_RESERVED(6), NULL, __func__, __FILE__, NULL, __LINE__, "passed NULL arg(s)");
	not_reached();
    }

    field_in_table = find_json_field_in_table(info_json_fields, name, &loc);
    if (field_in_table == NULL) {
	err(33, __func__, "called add_found_info_json_field() on field '%s' not specific "
			  "to .info.json in file %s", name, json_filename);
	not_reached();
    }
    /*
     * Set in table that it's found and increment the number of times it's been
     * seen.
     */
    info_json_fields[loc].count++;
    info_json_fields[loc].found = true;

    for (field = found_info_json_fields; field != NULL; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    /*
	     * we found a field already in the list: add the value (even if this
	     * value was already in the list as this is needed in some cases).
	     */
	    value = add_json_value(json_filename, field, val, line_num);
	    if (value == NULL) {
		/*
		 * this shouldn't happen as if add_json_value() gets an error
		 * it'll abort but just to be safe we check here too
		 */
		jerr(JSON_CODE_RESERVED(7), NULL, __func__, __FILE__, NULL, __LINE__,
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
	jerr(JSON_CODE_RESERVED(8), NULL, __func__, __FILE__, NULL, __LINE__,
				    "error creating new struct json_field * for field '%s' value '%s' file %s",
				    name, val, json_filename);
	not_reached();
    }


    /* add to the list */
    field->next = found_info_json_fields;
    found_info_json_fields = field;

    dbg(DBG_VHIGH, "added field '%s' value '%s'", field->name, val);

    return field;
}

/* add_manifest_file	- add filename to manifest_files_list list
 *
 * given:
 *
 *	filename	- name of file to add to manifest_files_list list
 *
 * This list is used to detect if any filenames are duplicates.
 *
 * NOTE: If the filename is already in the files list we only increment the
 * counter; we don't add a new struct manifest_file * to the list.
 *
 * If filename is NULL or empty value warn about it and return NULL.
 *
 * This function does not return on error.
 *
 */
static struct manifest_file *
add_manifest_file(char const *filename)
{
    struct manifest_file *file = NULL; /* to check if the manifest_file already is in the manifest_files_list */
    struct manifest_file *manifest_file = NULL; /* the newly allocated manifest_file * */

    /*
     * firewall
     */
    if (filename == NULL || strlen(filename) == 0) {
	warn(__func__, "passed NULL or empty filename");
	return NULL;
    }

    for (file = manifest_files_list; file != NULL; file = file->next)
    {
	if (!strcasecmp(file->filename, filename)) {
	    dbg(DBG_MED, "incrementing count of filename %s", filename);
	    file->count++;
	    return file;
	}
    }

    /* allocate a struct for the file */
    errno = 0;
    manifest_file = calloc(1, sizeof *manifest_file);
    if (manifest_file == NULL) {
	err(34, __func__, "calloc() error allocating struct manifest_file * for filename %s", filename);
	not_reached();
    }

    /* record the filename */
    errno = 0;
    manifest_file->filename = strdup(filename);
    if (manifest_file->filename == NULL) {
	err(35, __func__, "strdup() error on filename %s", filename);
	not_reached();
    }

    /*
     * we have to keep track of the number of times this file exists in the
     * manifest
     */
    manifest_file->count = 1;

    /* prepend it to the list */
    manifest_file->next = manifest_files_list;
    manifest_files_list = manifest_file;

    return manifest_file;
}

/* free_manifest_file	    - free a struct manifest_file *
 *
 * given:
 *
 *	file		    - the manifest_file * to free
 *
 * This function does not return on NULL pointer because a NULL pointer should
 * never be passed to the function.
 *
 */
static void
free_manifest_file(struct manifest_file *file)
{
    /*
     * firewall
     */
    if (file == NULL) {
	err(36, __func__, "passed NULL file");
	not_reached();
    }

    free(file->filename);
    file->filename = NULL;

    free(file);
    file = NULL; /* not helpful here but I always set pointers to NULL after freeing */
}


/* free_manifest_files_list	- free the manifest files list: manifest_files_list
 *
 * NOTE: If the manifest_files_list list is NULL this function does nothing.
 */
static void
free_manifest_files_list(void)
{
    struct manifest_file *file, *next_file;

    for (file = manifest_files_list; file != NULL; file = next_file) {
	next_file = file->next;

	free_manifest_file(file);

	file = NULL;
    }

    manifest_files_list = NULL;
}


/*
 * check_found_info_json_fields - found_info_json_fields table value checks
 *
 * Verify that all the fields in the found_info_json_fields table have values
 * that are valid and that all fields that are required are in the file.
 *
 *  given:
 *
 *	    json_filename   - .info.json file we're checking
 *	    test	    - if test mode: ignore some checks
 *
 *
 * returns:
 *	>0 ==> the number of issues found
 *	0 ==> if no issues were found
 */
int
check_found_info_json_fields(char const *json_filename, bool test)
{
    struct json_field *field; /* current field in found_info_json_fields list */
    struct json_value *value; /* current value in current field's values list */
    struct json_field *info_field = NULL; /* element in the info_json_fields table */
    struct manifest_file *manifest_file = NULL;
    size_t loc = 0;	/* location in the info_json_fields table */
    size_t val_length = 0;
    int issues = 0;

    /*
     * firewall
     */
    if (json_filename == NULL) {
	err(37, __func__, "passed NULL json_filename");
	not_reached();
    }

    for (field = found_info_json_fields; field != NULL; field = field->next) {
	/*
	 * first make sure the name != NULL and strlen() > 0
	 */
	if (field->name == NULL || strlen(field->name) <= 0) {
	    jerr(JSON_CODE_RESERVED(9), NULL, __func__, __FILE__, NULL, __LINE__,
				        "found NULL or empty field in found_info_json_fields list in file %s",
					json_filename);
	    not_reached();
	}

	/* now make sure it's allowed to be in this table. */
	loc = 0;
	info_field = find_json_field_in_table(info_json_fields, field->name, &loc);

	/*
	 * If the field is not allowed in the list then it suggests there is a
	 * problem in the code because only info fields should be added to the
	 * list in the first place. Thus it's an error if a field that's in the
	 * info list is not an info field name.
	 */
	if (info_field == NULL) {
	    jerr(JSON_CODE_RESERVED(10), NULL, __func__, __FILE__, NULL, __LINE__,
					 "illegal field name '%s' in found_info_json_fields list in file %s",
					 field->name, json_filename);
	    not_reached();
	}

	dbg(DBG_VHIGH, "checking field '%s' in file %s", field->name, json_filename);
	for (value = field->values; value != NULL; value = value->next) {
	    char *val = value->value;

	    if (val == NULL) {
		jerr(JSON_CODE_RESERVED(11), NULL, __func__, __FILE__, NULL, __LINE__,
					     "NULL pointer val for field '%s' in file %s",
					     field->name, json_filename);
		not_reached();
	    }

	    val_length = strlen(val);

	    if (val_length <= 0 && strcmp(field->name, "manifest")) {
		/*
		 * manifest has an empty value in a sense so we only do this for
		 * fields that aren't manifest.
		 */
		err(38, __func__, "empty value found for field '%s' in file %s", field->name, json_filename);
		not_reached();
	    }

	    /* make sure the field is not over the limit allowed */
	    if (info_field->max_count > 0 && info_field->count > info_field->max_count) {
		jwarn(JSON_CODE(1), program, __func__, __FILE__, NULL, value->line_num,
				    "field '%s' found %ju times but is only allowed %ju time%s in file %s",
				    info_field->name, (uintmax_t)info_field->count,
				    (uintmax_t)info_field->max_count,
				    info_field->max_count==1?"":"s", json_filename);
		++issues;
	    }


	    /*
	     * First we do checks on the field type. We only have to check
	     * numbers and bools: because for strings there's nothing to
	     * check: we remove the outermost '"'s and then use strcmp() whereas
	     * for numbers and bools we want to make sure that they're actually
	     * valid values.
	     *
	     * A note on the booleans checked later: perhaps a table could be
	     * used for easier assignment but for now we do strcmp() on each
	     * name individually.
	     */
	    switch (info_field->field_type) {
		case JTYPE_BOOL:
		    if (strcmp(val, "false") && strcmp(val, "true")) {
			warn(__func__, "bool field '%s' has non boolean value in file %s: '%s'",
				       info_field->name, json_filename, val);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "... %s is a bool", val);
		    }
		    break;
		case JTYPE_INT:
		    if (!is_integer(val)) {
			warn(__func__, "number field '%s' has non-number value in file %s: '%s'",
				       info_field->name, json_filename, val);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "... %s is an integer", val);
		    }
		    break;
		default:
		    break;
	    }

	    if (!strcmp(field->name, "IOCCC_info_version")) {
		if (!test && strcmp(val, INFO_VERSION)) {
		    warn(__func__, "IOCCC_info_version != INFO_VERSION \"%s\" in file %s: \"%s\"",
				   INFO_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "jinfochk_version")) {
		if (!test && strcmp(val, JINFOCHK_VERSION)) {
		    warn(__func__, "jinfochk_version != JINFOCHK_VERSION \"%s\" in file %s: \"%s\"",
				   JINFOCHK_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "txzchk_version")) {
		if (!test && strcmp(val, TXZCHK_VERSION)) {
		    warn(__func__, "txzchk_version != TXZCHK_VERSION \"%s\" in file %s: \"%s\"",
				    TXZCHK_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "iocccsize_version")) {
		if (!test && strcmp(val, IOCCCSIZE_VERSION)) {
		    warn(__func__, "iocccsize_version != IOCCCSIZE_VERSION \"%s\" in file %s: \"%s\"",
				   IOCCCSIZE_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "title")) {
		/* check for valid title length */
		if (!val_length) {
		    warn(__func__, "title length zero in file %s", json_filename);
		    ++issues;
		} else if (val_length > MAX_TITLE_LEN) {
		    warn(__func__, "title length %ju > max %d in file %s: '%s'",
				      (uintmax_t)val_length, MAX_TITLE_LEN, json_filename, val);
		    ++issues;
		}

		/* check for valid title chars */
		if (!posix_plus_safe(val, true, false, true)) {
		    warn(__func__, "title does not match regexp ^[0-9a-z][0-9a-z._+-]*$ in file %s: '%s'", json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "abstract")) {
		if (!val_length) {
		    warn(__func__, "abstract value zero length in file %s", json_filename);
		    ++issues;
		} else if (val_length > MAX_ABSTRACT_LEN) {
		    warn(__func__, "abstract length %ju > max %d in file %s: '%s'",
				      (uintmax_t)val_length, MAX_ABSTRACT_LEN, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "info_JSON")) {
		if (strcmp(val, ".info.json")) {
		    warn(__func__, "found invalid info_JSON value in file %s: '%s'", json_filename, val);
		    ++issues;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(39, __func__, "couldn't add info_JSON file '%s'", val);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "author_JSON")) {
		if (strcmp(val, ".author.json")) {
		    warn(__func__, "found invalid author_JSON value in file %s: '%s'", json_filename, val);
		    ++issues;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(40, __func__, "couldn't add author_JSON file '%s'", val);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "c_src")) {
		if (strcmp(val, "prog.c")) {
		    warn(__func__, "found invalid c_src value in file %s: '%s'", json_filename, val);
		    ++issues;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(41, __func__, "couldn't add c_src file '%s'", val);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "Makefile")) {
		if (strcmp(val, "Makefile")) {
		    warn(__func__, "found invalid Makefile value in file %s: '%s'", json_filename, val);
		    ++issues;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(42, __func__, "couldn't add Makefile file '%s'", val);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "remarks")) {
		if (strcmp(val, "remarks.md")) {
		    warn(__func__, "found invalid remarks value in file %s: '%s'", json_filename, val);
		    ++issues;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(43, __func__, "couldn't add remarks file '%s'", val);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "extra_file")) {
		if (val_length > MAX_BASENAME_LEN) {
		    warn(__func__, "extra file name length %ju > the limit %ju: '%s'",
				   (uintmax_t)val_length, (uintmax_t)MAX_BASENAME_LEN, val);
		    ++issues;
		}
	        /* extra_file must use only POSIX portable filename and + chars */
		if (!posix_plus_safe(val, false, false, true)) {
		    warn(__func__, "extra data file does not match regexp ^[0-9A-Za-z][0-9A-Za-z._+-]*$ "
				   "in file %s: '%s'", json_filename, val);
		    ++issues;
		    break;
		}
		manifest_file = add_manifest_file(val);
		if (manifest_file == NULL) {
		    err(44, __func__, "couldn't add extra_file file '%s' in file %s", val, json_filename);
		    not_reached();
		}
	    } else if (!strcmp(field->name, "rule_2a_size")) {
		info.rule_2a_size = string_to_long_long(val);
	    } else if (!strcmp(field->name, "rule_2b_size")) {
		info.rule_2b_size = string_to_unsigned_long_long(val);
	    } else if (!strcmp(field->name, "empty_override")) {
		info.empty_override = string_to_bool(val);
	    } else if (!strcmp(field->name, "rule_2a_override")) {
		info.rule_2a_override = string_to_bool(val);
	    } else if (!strcmp(field->name, "rule_2a_mismatch")) {
		info.rule_2a_mismatch = string_to_bool(val);
	    } else if (!strcmp(field->name, "rule_2b_override")) {
		info.rule_2b_override = string_to_bool(val);
	    } else if (!strcmp(field->name, "highbit_warning")) {
		info.highbit_warning = string_to_bool(val);
	    } else if (!strcmp(field->name, "nul_warning")) {
		info.nul_warning = string_to_bool(val);
	    } else if (!strcmp(field->name, "trigraph_warning")) {
		info.trigraph_warning = string_to_bool(val);
	    } else if (!strcmp(field->name, "wordbuf_warning")) {
		info.wordbuf_warning = string_to_bool(val);
	    } else if (!strcmp(field->name, "ungetc_warning")) {
		info.ungetc_warning = string_to_bool(val);
	    } else if (!strcmp(field->name, "Makefile_override")) {
		info.Makefile_override = string_to_bool(val);
	    } else if (!strcmp(field->name, "first_rule_is_all")) {
		info.first_rule_is_all = string_to_bool(val);
	    } else if (!strcmp(field->name, "found_all_rule")) {
		info.found_all_rule = string_to_bool(val);
	    } else if (!strcmp(field->name, "found_clean_rule")) {
		info.found_clean_rule = string_to_bool(val);
	    } else if (!strcmp(field->name, "found_clobber_rule")) {
		info.found_clobber_rule = string_to_bool(val);
	    } else if (!strcmp(field->name, "found_try_rule")) {
		info.found_try_rule = string_to_bool(val);
	    } else if (strcmp(field->name, "manifest")) {
		/*
		 * This should never actually be reached but if it is it
		 * suggests that a field was not added to be checked: if it's
		 * not a valid field we would have already detected and aborted
		 * earlier in this loop so we don't have to check for that.
		 */
		warn(__func__, "found unhandled info field in file %s: '%s'", json_filename, field->name);
		/*
		 * NOTE: Don't increment issues because this doesn't mean
		 * there's anything wrong with the .info.json file but rather
		 * that the field isn't verified.
		 */
	    }
	}
    }

    /*
     * Now we have to do some additional sanity tests like bool mismatches etc.
     *
     * If Makefile override is set to true and there are no problems found with
     * the Makefile there's a mismatch: check and report if this is the case.
     */
    if (info.Makefile_override && info.first_rule_is_all && info.found_all_rule &&
	    info.found_clean_rule && info.found_clobber_rule && info.found_try_rule) {
	warn(__func__, "Makefile_override == true but all expected Makefile rules found "
		       "and 'all:' is first in file %s", json_filename);
	++issues;
    }

    /*
     * If info.found_all_rule == false and info.first_rule_is_all == true
     * there's a mismatch: check this and report if this is the case.
     */
    if (!info.found_all_rule && info.first_rule_is_all) {
	warn(__func__, "'all:' rule not found but first_rule_is_all == true in file %s", json_filename);
	++issues;
    }

    /* if empty_override == true and prog.c is not size 0 there's a problem */
    if (info.empty_override && info.rule_2a_size > 0 && info.rule_2b_size > 0) {
	warn(__func__, "empty_override == true but prog.c size > 0 in file %s", json_filename);
	++issues;
    }

    /*
     * If empty_override == false and either of rule 2a or rule 2b size == 0
     * there's a problem.
     */
    if (!info.empty_override && (info.rule_2a_size == 0 || info.rule_2b_size == 0)) {
	warn(__func__, "empty_override == false but rule 2a and/or rule 2b size == 0 in file %s", json_filename);
	++issues;
    }

    /*
     * Now that we've checked each field by name, we still have to make sure
     * that each field expected is actually there. Note that in the above loop
     * we already tested if each field has been seen more times than allowed so
     * we don't do that here. This is because the fields that are in the list
     * are those that will potentially have more than allowed whereas here we're
     * making sure every field that is required is actually in the list.
     */
    for (loc = 0; info_json_fields[loc].name != NULL; ++loc) {
	if (!info_json_fields[loc].found && info_json_fields[loc].max_count > 0) {
	    warn(__func__, "required field not found in found_info_json_fields list "
			   "in file %s: '%s'", json_filename, info_json_fields[loc].name);
	    ++issues;
	}
    }

    /*
     * Check for duplicate files in the manifest.
     *
     * XXX - This should probably be in its own function.
     */
    for (manifest_file = manifest_files_list; manifest_file != NULL; manifest_file = manifest_file->next) {
	if (manifest_file->count > 1) {
	    warn(__func__, "found duplicate file (count: %ju) in file %s: '%s'",
			   (uintmax_t)manifest_file->count, json_filename, manifest_file->filename);
	    ++issues;
	}
    }

    return issues;
}


/*
 * free_found_info_json_fields - free the found_info_json_fields list
 */
static void
free_found_info_json_fields(void)
{
    struct json_field *field, *next_field = NULL;

    for (field = found_info_json_fields; field != NULL; field = next_field) {
	next_field = field->next;
	free_json_field(field);
	field = NULL;
    }
    found_info_json_fields = NULL; /* NULL out list */
}



/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: %s", program);
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
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FNAMCHK_PATH_0, JINFOCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}

int
main(int argc, char **argv)
{
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *file;			/* file argument to check */
    int ret;			/* libc return code */
    int issues = 0;
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false; /* true ==> -F fnamchk used */
    int code;			/* a JSON error code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    program_basename = base_name(program);
    while ((i = getopt(argc, argv, "hv:qVSF:tW:w")) != -1) {
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
	    ret = printf("%s\n", JINFOCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JINFOCHK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'S':
	    strict = true;
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
	ret = printf("Welcome to jinfochk version: %s\n", JINFOCHK_VERSION);
	if (ret <= 0) {
	    errp(45, __func__, "printf error printing the welcome string");
	    not_reached();
	}

	/*
	 * environment sanity checks
	 */
	para("", "Performing sanity checks on your environment ...", NULL);
    }

    /* we have to find the fnamchk util */
    find_utils(false, NULL, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk,
	       false, NULL, false, NULL);


    jinfochk_sanity_chks(file, fnamchk);
    if (!quiet) {
	para("... environment looks OK", "", NULL);
    }

    issues = check_info_json(file, fnamchk);

    if (program_basename != NULL) {
	free(program_basename);
	program_basename = NULL;
    }

    /* free any allocated memory in our info struct */
    free_info(&info);

    if (issues != 0 && !test) {
	dbg(DBG_LOW, "%s is invalid", file);
    }
    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(issues != 0); /*ooo*/
}
