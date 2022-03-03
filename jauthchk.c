/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jauthchk - IOCCC JSON .author.json checker and validator
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
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
 * .author.json fields table used to determine if a name belongs in the file,
 * whether it's been added to the found_author_json_fields list, how many times
 * it's been seen and how many are allowed.
 *
 * XXX: As of 27 February 2022 all fields are in the table but because arrays
 * are not yet parsed not all of these values will be dealt with: that is they
 * won't be in the found_author_json list. Additionally the way array elements
 * are defined might very well change.
 */
struct json_field author_json_fields[] =
{
    { "IOCCC_author_version",	NULL, 0, 1, false, JSON_STRING,		NULL },
    { "authors",		NULL, 0, 1, false, JSON_ARRAY,		NULL },
    { "name",			NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "location_code",		NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "email",			NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "url",			NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "twitter",		NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "github",			NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "affiliation",		NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "author_handle",		NULL, 0, 5, false, JSON_ARRAY_STRING,	NULL },
    { "author_number",		NULL, 0, 5, false, JSON_ARRAY_NUMBER,	NULL },
    { NULL,			NULL, 0, 0, false, JSON_NULL,		NULL } /* this **MUST** be last */
};


int
main(int argc, char **argv)
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *file;				/* file argument to check */
    int ret;				/* libc return code */
    int i;				/* return value of getopt() */
    int issues;				/* issues found */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false;	/* true ==> -F fnamchk used */


    /*
     * parse args
     */
    program = argv[0];
    program_basename = base_name(program);
    while ((i = getopt(argc, argv, "hv:VqsF:tT")) != -1) {
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
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", JAUTHCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JAUTHCHK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'T':		/* -T (IOCCC toolset chain release repository tag) */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", IOCCC_TOOLSET_RELEASE);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing IOCCC toolset release repository tag");
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    break;
	case 's':
	    strict = true;
	    break;
	case 'F':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't':
	    test = true;
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
	    errp(4, __func__, "printf error printing the welcome string");
	    not_reached();
	}
    }

    /* we have to find the fnamchk util */
    find_utils(false, NULL, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk,
	       false, NULL, false, NULL);

    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }

    sanity_chk(file, fnamchk);
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
     * free_author_array()
     */

    if (issues != 0 && !test) {
	dbg(DBG_LOW, "%s is invalid", file);
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(issues != 0 && !test); /*ooo*/
}


/*
 * check_author_json_fields_table - perform author_json_fields table sanity checks
 *
 * This function checks if JSON_NULL is used on any field other than the NULL
 * field. It also makes sure that each field_type is valid.  Additionally it
 * makes sure that there are no NULL elements before the final element.
 *
 * The checks are performed on the author_json_fields table.
 *
 * NOTE: More tests might be devised later on but this is a good start (28 Feb
 * 2022).
 *
 * This function does not return on error.
 */
static void
check_author_json_fields_table(void)
{
    size_t loc;
    size_t max = sizeof(author_json_fields)/sizeof(author_json_fields[0]);

    for (loc = 0; loc < max - 1 && author_json_fields[loc].name != NULL; ++loc) {
	switch (author_json_fields[loc].field_type) {
	    case JSON_NULL:
		if (author_json_fields[loc].name != NULL) {
		    err(5, __func__, "found JSON_NULL element with non NULL name '%s' location %lu in author_json_fields table",
                            author_json_fields[loc].name, (unsigned long)loc);
		    not_reached();
		}
		break;
	    case JSON_NUMBER:
	    case JSON_BOOL:
	    case JSON_STRING:
	    case JSON_ARRAY:
	    case JSON_ARRAY_NUMBER:
	    case JSON_ARRAY_BOOL:
	    case JSON_ARRAY_STRING:
		/* these are all the valid types */
		break;
	    default:
		err(6, __func__, "found invalid data_type in author_json_fields table location %lu", (unsigned long)loc);
		not_reached();
		break;
	}
    }

    if (max - 1 != loc) {
	err(7, __func__, "found embedded NULL element in author_json_fields table");
	not_reached();
    }

}


/*
 * sanity_chk - perform basic sanity checks
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
sanity_chk(char const *file, char const *fnamchk)
{
    /*
     * firewall
     */
    if (file == NULL || fnamchk == NULL) {
	err(8, __func__, "called with NULL arg(s)");
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
	err(9, __func__, "file does not exist: %s", file);
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
	err(10, __func__, "file is not a file: %s", file);
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
	err(11, __func__, "file is not readable: %s", file);
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
	      "A fnamchk program performs a sanity check on the compressed tarball filename.",
	      "Perhaps you need to use:",
	      "",
	      "    jauthchk -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(12, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it exists, is not a file.",
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
	err(13, __func__, "fnamchk is not a file: %s", fnamchk);
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
	err(14, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }

    /* check the common_json_fields table */
    check_common_json_fields_table();

    /* check our author_json_fields table */
    check_author_json_fields_table();

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
    char *val_esc = NULL; /* for malloc_json_decode_str() on val */
    char *saveptr = NULL; /* for strtok_r() usage */
    char *array = NULL; /* array extraction */
    char *array_start = NULL; /* start of array */
    char *array_dup = NULL; /* strdup()d copy of array */
    struct json_field *author_field; /* temporary use to determine type of value if .author.json field */
    struct json_field *common_field; /* temporary use to determine type of value if common field */
    size_t loc = 0;

    /*
     * firewall
     */
    if (file == NULL || fnamchk == NULL) {
	err(15, __func__, "passed NULL arg(s)");
	not_reached();
    }
    stream = fopen(file, "r");
    if (stream == NULL) {
	err(16, __func__, "couldn't open %s", file);
	not_reached();
    }

    /* read in the file */
    data = read_all(stream, &length);
    if (data == NULL) {
	err(17, __func__, "error while reading data in %s", file);
	not_reached();
    } else if (length == 0) {
	err(18, __func__, "zero length data in file %s", file);
	not_reached();
    }
    dbg(DBG_MED, "%s read length: %lu", file, (unsigned long)length);

    /* close the stream as we no longer need it, having read in all the file */
    errno = 0;
    ret = fclose(stream);
    if (ret != 0) {
	warnp(__func__, "error in fclose to %s file %s", json_filename(AUTHOR_JSON), file);
    }

    /* scan for embedded NUL bytes (before EOF) */
    if (!is_string(data, length+1)) {
	err(19, __func__, "found NUL before EOF: %s", file);
	not_reached();
    }

    errno = 0;
    data_dup = strdup(data);
    if (data_dup == NULL) {
	errp(20, __func__, "unable to strdup file contents");
	not_reached();
    }

    /*
     * Verify that the very last character is a '}'. We do this check first
     * because we don't care what comes after it (in parsing) but we do care
     * what comes after the '{' (in parsing); that is to say we proceed in
     * parsing after the first '{' but after the '}' we don't continue.
     */
    if (check_last_json_char(file, data_dup, strict, &p, '}')) {
	err(21, __func__, "last character in file %s not a '}': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_MED, "last character: '%c'", *p);

    /* remove closing brace (and any whitespace after it) */
    *p = '\0';

    /*
     * Check that the new last char is NOT a ','. Don't do strict checking
     * because we want the end spaces to be trimmed off first.
     */
    if (!check_last_json_char(file, data_dup, false, &p, ',')) {
	err(22, __func__, "last char is a ',' in file %s", file);
	not_reached();
    }


    /* verify that the very first character is a '{' */
    if (check_first_json_char(file, data_dup, strict, &p, '{')) {
	err(23, __func__, "first character in file %s not a '{': '%c'", file, *p);
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

	/* remove a single '"' if one exists at the beginning (*p == '"') */
	if (*p == '"')
	    ++p;

	/* if empty string, break out of loop */
	if (!*p)
	    break;

	/* also skip trailing spaces */
	end = p + strlen(p) - 1;
	while (*end && isspace(*end))
	    *end-- = '\0';

	/* remove a single '"' if one exists at the end (*end == '"') */
	if (*end == '"')
	    *end = '\0';


	/* if string is empty break out of loop */
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
	 */
	if (author_field == NULL && common_field == NULL) {
	    err(24, __func__, "invalid field '%s'", p);
	    not_reached();
	}

	if (!strcmp(p, "authors")) {
	    /* TODO The below only extracts the array: it does not yet parse it.
	     * This will come in a future commit.
	     */
	    if (!author_field) {
		err(25, __func__, "authors field not found in author_json_fields table");
		not_reached();
	    }

	    /* find start of array */
	    array_start = strtok_r(NULL, ":[{", &saveptr);
	    if (array_start == NULL) {
		err(26, __func__, "unable to find beginning of array");
		not_reached();
	    }

	    /* if empty string break out of loop */
	    if (!*array_start)
		break;

	    /* extract the array */
	    array = strtok_r(NULL, "]", &saveptr);
	    if (array == NULL) {
		err(27, __func__, "unable to extract array in file %s", file);
		not_reached();
	    }

	    if (!*array) {
		err(28, __func__, "empty array in file %s", file);
		not_reached();
	    }

	    /* strdup() the array for special parsing */
	    errno = 0;
	    array_dup = strdup(array);
	    if (array_dup == NULL) {
		errp(29, __func__, "strdup() on array failed: %s", strerror(errno));
		not_reached();
	    }
	    /* Update p to go beyond the array. */
	    p = array + strlen(array) + 1;
	} else {
	    /* extract the value */
	    val = strtok_r(NULL, ",", &saveptr);
	    if (val == NULL) {
		err(30, __func__, "unable to find value in file %s for field %s", file, p);
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

	    /*
	     * If the field type is a string we have to remove a single '"' and
	     * from the beginning and end of the value.
	     */
	    if ((common_field && (common_field->field_type == JSON_STRING || common_field->field_type == JSON_ARRAY_STRING)) ||
		(author_field && (author_field->field_type == JSON_STRING || author_field->field_type == JSON_ARRAY_STRING))) {
		    /* remove a single '"' at the beginning of the value */
		    if (*val == '"')
			++val;

		    /* if nothing left break out of loop */
		    if (!*val)
			break;

		    /* also remove a trailing '"' at the end of the value. */
		    end = val + strlen(val) - 1;
		    if (*end == '"')
			*end = '\0';

		    /* if nothing left break out of loop */
		    if (!*val)
			break;

		    /*
		     * after removing the spaces and a single '"' at the beginning and end,
		     * if we find a '"' in the field we know it's erroneous.
		     */
	    }

	    /*
	     * We have to determine if characters were properly escaped
	     * according to the JSON spec.
	     */
	    val_esc = malloc_json_decode_str(val, NULL, strict);
	    if (val_esc == NULL) {
		err(31, __func__, "malloc_json_decode(): invalidly formed field '%s' value '%s' or malloc failure in file %s",
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
	    if (get_common_json_field(program_basename, file, p, val_esc)) {
	    } else if (get_author_json_field(file, p, val_esc)) {
	    } else {
		/* this should actually never be reached */
		warn(__func__, "invalid field '%s' found in file %s", p, file);
		++issues;
	    }
	    
	    /* free the JSON decoded value */
	    free(val_esc);
	    val_esc = NULL;
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
    issues += check_found_common_json_fields(program_basename, file, fnamchk, test);

    /* free the found_common_json_fields list.
     *
     * NOTE: After this the list will be NULL.
     */
    free_found_common_json_fields();

    /* if issues != 0 there will be a non-zero return status of jauthchk */
    return issues;
}


/*
 * get_author_json_field - check field name
 *
 * Check if name is a .info.json field, and check if name is found
 * in the found_author_json list.
 *
 * given:
 *
 *	file	- the file being parsed (path to)
 *	name	- the field name
 *	val	- the value of the field
 *
 * returns:
 *	1 ==> if the name is a .info.json field
 *	0 ==> if it's not one of the .info.json fields
 *
 * NOTE: Does not return on error (NULL pointers).
 */
int
get_author_json_field(char const *file, char *name, char *val)
{
    int ret = 1;	/* return value: 1 ==> known field, 0 ==> not a common field */
    struct json_field *field = NULL; /* the field in the author_json_fields table if found */
    size_t loc = 0; /* location in the author_json_fields table */

    /*
     * firewall
     */
    if (file == NULL || name == NULL || val == NULL) {
	err(32, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * check if the name is an expected common field
     */
    field = find_json_field_in_table(author_json_fields, name, &loc);
    if (field != NULL) {
	dbg(DBG_MED, "found field '%s' with value '%s'", field->name, val);
	add_found_author_json_field(field->name, val);
    } else {
	ret = 0;
    }
    return ret;
}


/*
 * check_found_author_json_fields - found_author_json_fields table value check
 *
 * Verify that all the fields in the found_author_json_fields table have
 * values that are valid and that all fields that are required are in the file
 * (TODO as of 3 March 2022 this is not actually done because arrays are not yet
 * parsed: we have to extract all fields first before this test can be added).
 *
 *  given:
 *
 *	    file	- .author.json file we're checking
 *	    test	- if test mode: ignore some checks
 *
 *
 * returns:
 *	>0 ==> the number of issues found
 *	0 ==> if no issues were found
 */
int
check_found_author_json_fields(char const *file, bool test)
{
    struct json_field *field; /* current field in found_author_json_fields list */
    struct json_value *value; /* current value in current field's values list */
    struct json_field *author_field = NULL; /* element in the author_json_fields table */
    size_t loc = 0;	/* location in the author_json_fields table */
    int issues = 0;
    size_t val_length = 0;  /* current value length */

    /*
     * firewall
     */
    if (file == NULL) {
	err(33, __func__, "passed NULL file");
	not_reached();
    }

    for (field = found_author_json_fields; field != NULL; field = field->next) {
	/*
	 * first make sure the name != NULL and strlen() > 0
	 */
	if (field->name == NULL || !strlen(field->name)) {
	    err(34, __func__, "found NULL or empty field in found_author_json_fields list");
	    not_reached();
	}

	/* now make sure it's allowed to be in this table. */
	loc = 0;
	author_field = find_json_field_in_table(author_json_fields, field->name, &loc);

	/*
	 * If the field is not allowed in the list then it suggests there is a
	 * problem in the code because only author fields should be added to the
	 * list in the first place. Thus it's an error if a field that's in the
	 * author list is not a author field name.
	 */
	if (author_field == NULL) {
	    err(35, __func__, "illegal field name '%s' in found_author_json_fields list", field->name);
	    not_reached();
	}

	dbg(DBG_VHIGH, "checking field '%s' in file %s", field->name, file);
	/* make sure the field is not over the limit allowed */
	if (author_field->max_count > 0 && author_field->count > author_field->max_count) {
	    warn(__func__, "field '%s' found %lu times but is only allowed %lu time%s", author_field->name,
		    (unsigned long)author_field->count, (unsigned long)author_field->max_count, author_field->max_count==1?"":"s");
	    ++issues;
	}

	for (value = field->values; value != NULL; value = value->next) {
	    char *val = value->value;
	    val_length = strlen(val);

	    if (!val_length) {
		warn(__func__, "empty value found for field '%s' in file %s", field->name, file);
		/* don't increase issues because the below checks will do that
		 * too: this warning only notes the reason the test will fail.
		 */
	    }

	    /*
	     * First we do checks on the field type. We only have to check
	     * numbers and bools: because for strings there's nothing to
	     * check: we remove the outermost '"'s and then use strcmp() whereas
	     * for numbers and bools we want to make sure that they're actually
	     * valid values.
	     */
	    switch (author_field->field_type) {
		case JSON_BOOL:
		    if (strcmp(val, "false") && strcmp(val, "true")) {
			warn(__func__, "bool field '%s' has invalid value '%s' in file %s", author_field->name, val, file);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "%s is a bool", val);
		    }
		    break;
		case JSON_ARRAY_BOOL:
		    break; /* arrays are not handled yet */
		case JSON_NUMBER:
		    if (!is_number(val)) {
			warn(__func__, "number field '%s' has non-number value '%s' in file %s", author_field->name, val, file);
			++issues;
			continue;
		    } else {
			dbg(DBG_VHIGH, "%s is a number", val);
		    }
		    break;
		case JSON_ARRAY_NUMBER:
		    break; /* arrays are not handled yet */
		default:
		    break;
	    }

	    if (!strcmp(field->name, "IOCCC_author_version")) {
		if (!test && strcmp(val, AUTHOR_VERSION)) {
		    warn(__func__, "IOCCC_author_version \"%s\" != \"%s\" in file %s", val, AUTHOR_VERSION, file);
		    ++issues;
		}
	    } else {
		/* this should never actually be reached but just in case */
		warn(__func__, "found invalid field '%s'", field->name);
		++issues;
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
add_found_author_json_field(char const *name, char const *val)
{
    struct json_field *field = NULL; /* iterate through fields list to find the field (or if not found, create a new field) */
    struct json_value *value = NULL; /* the new value */
    struct json_field *field_in_table = NULL;
    size_t loc = 0; /* location in author_json_fields table */

    /*
     * firewall
     */
    if (name == NULL || val == NULL) {
	err(36, __func__, "passed NULL arg(s)");
	not_reached();
    }

    field_in_table = find_json_field_in_table(author_json_fields, name, &loc);
    if (field_in_table == NULL) {
	err(37, __func__, "called add_found_author_json_field() on field '%s' not specific to .author.json", name);
	not_reached();
    }
    /*
     * Set in table that it's found and increment the number of times it's been
     * seen.
     */
    author_json_fields[loc].count++;
    author_json_fields[loc].found = true;


    for (field = found_author_json_fields; field; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    /*
	     * we found a field already in the list, add the value (even if this
	     * value was already in the list as this might need to be reported).
	     */
	    value = add_json_value(field, val);
	    if (value == NULL) {
		/*
		 * this shouldn't happen as if add_json_value() gets an error
		 * it'll abort but just to be safe we check here too
		 */
		err(38, __func__, "error adding json value '%s' to field '%s'", val, field->name);
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
    field = new_json_field(name, val);
    if (field == NULL) {
	/*
	 * we should never get here because if new_json_field gets NULL it
	 * aborts the program.
	 */
	err(39, __func__, "error creating new struct json_field * for field '%s' value '%s'", name, val);
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
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FNAMCHK_PATH_0, JAUTHCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
