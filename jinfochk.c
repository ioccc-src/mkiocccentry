/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jinfochk - IOCCC JSON .info.json checker and validator
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
#include "jinfochk.h"


int
main(int argc, char **argv)
{
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *file;			/* file argument to check */
    int ret;			/* libc return code */
    int i;
    int issues = 0;
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false; /* true ==> -F fnamchk used */

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
	    ret = printf("%s\n", JINFOCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JINFOCHK_VERSION);
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
	ret = printf("Welcome to jinfochk version: %s\n", JINFOCHK_VERSION);
	if (ret <= 0) {
	    errp(4, __func__, "printf error printing the welcome string");
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


    sanity_chk(file, fnamchk);
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

    if (issues != 0) {
	dbg(DBG_LOW, "%s is invalid", file);
    }
    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(issues != 0); /*ooo*/
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
	      "We cannot find a fnamchk tool.",
	      "",
	      "A fnamchk program performs a sanity check on the compressed tarball.",
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
    char *p = NULL;	/* temporary use: check for NUL bytes and field extraction */
    char *end = NULL;	/* temporary use: end of strings (p, field) for removing spaces */
    char *val = NULL;	/* current field's value being parsed */
    char *savefield = NULL; /* for strtok_r() usage */
    size_t val_length;    /* length of current val */
    size_t span;
    struct json_field *field; /* for found_info_json_fields list */
    struct json_value *value; /* for found_info_json_fields list's value */

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
    dbg(DBG_HIGH, "%s read length: %lu", file, (unsigned long)length);

    /* close the stream as we no longer need it, having read in all the file */
    errno = 0;
    ret = fclose(stream);
    if (ret != 0) {
	warnp(__func__, "error in fclose to %s file %s", json_filename(INFO_JSON), file);
    }

    /* scan for embedded NUL bytes (before EOF) */
    errno = 0;			/* pre-clear errno for errp() */
    p = (char *)memchr(data, 0, (size_t)length);
    if (p != NULL) {
	errp(16, __func__, "found NUL before EOF: %s", file);
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
    if (check_last_json_char(file, data_dup, strict, &p)) {
	err(18, __func__, "last character in file %s not a '}': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_HIGH, "last character: '%c'", *p);

    /* remove closing brace (and any whitespace after it) */
    *p = '\0';

    /* verify that the very first character is a '{' */
    if (check_first_json_char(file, data_dup, strict, &p)) {
	err(19, __func__, "first character in file %s not a '{': '%c'", file, *p);
	not_reached();
    }
    dbg(DBG_HIGH, "first character: '%c'", *p);

    /* skip past the initial opening brace */
    ++p;

    /*
     * Begin to parse the file, field by field (if it's not a validly formed
     * JSON file an error will occur and the loop will be aborted).
     */
    do {
	/* we have to skip leading whitespace */
	while (*p && isspace(*p))
	    ++p;
	/* get the next field */
	p = strtok_r(val?NULL:p, ":", &savefield);
	if (p == NULL) {
	    break;
	}

	/* skip leading whitespace on the field */
	while (*p && isspace(*p))
	    ++p;

	/* remove a single '"' if one exists at the beginning (*p == '"') */
	if (*p == '"')
	    ++p;

	/* also skip trailing spaces */
	end = p + strlen(p) - 1;
	while (*end && isspace(*end))
	    *end-- = '\0';

	/* remove a single '"' if one exists at the end (*end == '"') */
	if (*end == '"')
	    *end = '\0';


	/*
	 * after removing the spaces and a single '"' at the beginning and end,
	 * if we find a '"' in the field we know it's erroneous: thus we can
	 * simply use strcmp() on it. Note that when we get to the array(s) we
	 * have to handle it specially but this step still has to be done.
	 */

	/*
	 * Before we can extract the value we have to determine if the field is
	 * supposed to be an array or not: if it's an array we have to handle it
	 * differently as there's more to parse. If it's not an array we just
	 * check the name, retrieve the value and then test if it's a valid
	 * value.
	 */
	if (!strcmp(p, "manifest")) {
	    /* TODO: handle the array */

	    /* The below is only done to prevent infinite loop which occurs in
	     * some cases (e.g. when "manifest" is at the top of the file) until
	     * arrays are handled; when arrays are handled this will be changed.
	     */
	    val = strtok_r(NULL, ",\0", &savefield);
	    if (val == NULL) {
		err(20, __func__, "unable to find value in file %s for field %s", file, p);
		not_reached();
	    }
	} else {
	    /* extract the value */
	    val = strtok_r(NULL, ",\0", &savefield);
	    if (val == NULL) {
		err(21, __func__, "unable to find value in file %s for field %s", file, p);
		not_reached();
	    }


	    /* skip leading whitespace */
	    while (*val && isspace(*val))
		++val;

	    /* skip trailing whitespace */
	    end = val + strlen(val) - 1;
	    while (*end && isspace(*end))
		*end-- = '\0';

	    /*
	     * Depending on the field, remove a single '"' at the beginning and
	     * end of the value.
	     */
	    if (strcmp(p, "ioccc_year") && strcmp(p, "entry_num") && strcmp(p, "rule_2a_size") &&
		strcmp(p, "rule_2b_size") && strcmp(p, "empty_override") && strcmp(p, "rule_2a_override") &&
		strcmp(p, "rule_2a_mismatch") && strcmp(p, "rule_2b_override") && strcmp(p, "highbit_warning") &&
		strcmp(p, "nul_warning") && strcmp(p, "trigraph_warning") && strcmp(p, "wordbuf_warning") &&
		strcmp(p, "ungetc_warning") && strcmp(p, "Makefile_override") && strcmp(p, "first_rule_is_all") &&
		strcmp(p, "found_all_rule") && strcmp(p, "found_clean_rule") && strcmp(p, "found_clobber_rule") &&
		strcmp(p, "found_try_rule") && strcmp(p, "test_mode")) {
		    /* remove a single '"' at the beginning of the value */
		    if (*val == '"')
			++val;

		    /* also remove a trailing '"' at the end of the value. */
		    end = val + strlen(val) - 1;
		    if (*end == '"')
			*end = '\0';

		    /*
		     * after removing the spaces and a single '"' at the beginning and end,
		     * if we find a '"' in the field we know it's erroneous.
		     */
	    }
	    val_length = strlen(val);
	    /* handle regular field */
	    if (get_common_json_field(program_basename, file, p, val)) {
		dbg(DBG_HIGH, "found common field '%s' value '%s'", p, val);
	    } else {
		field = add_found_info_json_field(p, val);
		if (field == NULL) {
		    /*
		     * if this is NULL there's a serious problem as the other
		     * functions should have aborted already
		     */
		    err(22, __func__, "couldn't add field '%s' with value '%s' to list", p, val);
		    not_reached();
		}
		dbg(DBG_HIGH, "found field '%s' with value '%s'", p, val);
	    } /* uncommon value: value specific to .info.json */
	}
    } while (true); /* end do while */

    /* free data */
    free(data);
    data = NULL;

    /* free strdup()d data */
    free(data_dup);
    data_dup = NULL;


    /*
     * now iterate through the found_info_json_fields list, reporting any issues
     *
     * XXX Note that fields that aren't expected to be in the file would also be
     * added to the list but this will be dealt with at a later time.
     */
    for (field = found_info_json_fields; field != NULL; field = field->next) {
	dbg(DBG_VHIGH, "checking field '%s' in file %s", field->name, file);
	for (value = field->values; value != NULL; value = value->next) {
	    char const *v = value->value;
	    val_length = strlen(v);

	    if (!strcmp(field->name, "IOCCC_info_version")) {
		if (!test && strcmp(v, INFO_VERSION)) {
		    warn(__func__, "IOCCC_info_version \"%s\" != \"%s\" in file %s", v, INFO_VERSION, file);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "title")) {
		if (val == 0) {
		    warn(__func__, "title length zero");
		    ++issues;
		} else if (val_length > MAX_TITLE_LEN) {
		    warn(__func__, "title length %lu > max %d",
				      (unsigned long)val_length, MAX_TITLE_LEN);
		    ++issues;
		}

		/* check for valid chars only */
		if (!isascii(v[0]) || (!islower(v[0]) && !isdigit(v[0]))) {
		    warn(__func__, "first char of title '%c' invalid", v[0]);
		    ++issues;
		} else {
		    span = strspn(v, TAIL_TITLE_CHARS);
		    if (span != val_length) {
			warn(__func__, "invalid chars found in title \"%s\"", v);
			++issues;
		    }
		}
	    } else if (!strcmp(field->name, "abstract")) {
		if (val_length == 0) {
		    warn(__func__, "abstract value zero length");
		    ++issues;
		} else if (val_length > MAX_ABSTRACT_LEN) {
		    warn(__func__, "abstract length %lu > max %d",
				      (unsigned long)val_length, MAX_ABSTRACT_LEN);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "rule_2a_size")) {
		info.rule_2a_size = string_to_long_long(v);
	    } else if (!strcmp(field->name, "rule_2b_size")) {
		info.rule_2b_size = string_to_unsigned_long_long(v);
	    } else if (!strcmp(field->name, "empty_override") || !strcmp(field->name, "rule_2a_override") ||
	      !strcmp(field->name, "rule_2a_mismatch") || !strcmp(field->name, "rule_2b_override") ||
	      !strcmp(field->name, "highbit_warning") || !strcmp(field->name, "nul_warning") ||
	      !strcmp(field->name, "trigraph_warning") || !strcmp(field->name, "wordbuf_warning") ||
	      !strcmp(field->name, "ungetc_warning") || !strcmp(field->name, "Makefile_override") ||
	      !strcmp(field->name, "first_rule_is_all") || !strcmp(field->name, "found_all_rule") ||
	      !strcmp(field->name, "found_clean_rule") || !strcmp(field->name, "found_clobber_rule") ||
	      !strcmp(field->name, "found_try_rule") || !strcmp(field->name, "test_mode")) {
		if (strcmp(v, "false") && strcmp(v, "true")) {
		    warn(__func__, "found non-boolean value '%s' for boolean '%s' in file %s", v,  field->name, file);
		    ++issues;
		}
	    }
	    else {
		/* TODO: after everything else is parsed if we get here it's an
		 * error as there's an invalid field in the file.
		 *
		 * Currently (as of 25 February 2022) this is not done
		 * because the arrays are not parsed yet.
		 */

	    }
	}
    }

    /* now free the found_info_json_fields list.
     *
     * NOTE: after this the list will be NULL
     */
    free_found_info_json_fields();

    issues += check_found_common_json_fields(program_basename, file, fnamchk, test);

    /* free the found_common_json_fields list.
     *
     * NOTE: After this the list will be NULL
     */
    free_found_common_json_fields();

    /* if issues != 0 there will be a non-zero return status of jinfochk */
    return issues;
}

/* add_found_info_json_field	- add a field:value pair to the
 *				  found_info_json_fields list
 *
 * given:
 *
 *	name			- field name
 *	val			- field value
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
add_found_info_json_field(char const *name, char const *val)
{
    struct json_field *field = NULL; /* iterate through fields list to find the field (or if not found, create a new field) */
    struct json_value *value = NULL; /* the new value */

    /*
     * firewall
     */
    if (name == NULL || val == NULL) {
	err(23, __func__, "passed NULL arg(s)");
	not_reached();
    }

    for (field = found_info_json_fields; field; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    /*
	     * we found a field already in the list: add the value (even if this
	     * value was already in the list as this might need to be reported).
	     */
	    value = add_json_value(field, val);
	    if (value == NULL) {
		/*
		 * this shouldn't happen as if add_json_value() gets an error
		 * it'll abort but just to be safe we check here too
		 */
		err(24, __func__, "error adding json value '%s' to field '%s'", val, field->name);
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
	err(25, __func__, "error creating new struct json_field * for field '%s' value '%s'", name, val);
	not_reached();
    }


    /* add to the list */
    field->next = found_info_json_fields;
    found_info_json_fields = field;

    dbg(DBG_VHIGH, "added field '%s' value '%s'", field->name, val);

    return field;
}

/* free_found_info_json_fields  - free the infos json fields list
 *
 * This function returns void.
 *
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
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FNAMCHK_PATH_0, JINFOCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
