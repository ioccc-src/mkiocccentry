/*
 * JSON scanner - JSON scanner in flex grammar form
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON scanner was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */

/* Section 1: Declarations and option settings */
/*
 * noyywrap prevents needing to link in the flex(1) library which means those
 * without flex can compile the code. Even if everyone had flex(1) though under
 * macOS you have to pass -ll to the compiler to link in the flex library
 * whereas in other systems you have to use -lfl which would cause a lot of
 * problems. Another way is to provide 'int yywrap() { return 1; }' but this is
 * unnecessary.
 */
%option noyywrap yylineno nodefault 8bit
%option bison-bridge bison-locations

%{
/* Declarations etc. go here.
 *
 * Code is copied verbatim near the top of the generated code.
 */


/*
 * jparse - JSON parser demo tool
 */
#include "jparse.h"


YY_BUFFER_STATE bs;

/*
 * XXX - FIXME - handle locations - FIXME - XXX
 *
 * This is just a placeholder for now and is subject to change.
 */
int yycolumn = 1;
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
    yycolumn += yyleng;
%}

/*
 * Section 2: Patterns (regular expressions) and actions.
 */

/*
 * XXX JSON_WS is not needed but for testing I have the whitespace here
 * and below in the actions I print out that it is whitespace and how many are
 * being ignored. The text looks like one of:
 *
 *	ignoring 1 whitespace
 *	ignoring [2-9]+ whitespaces
 *
 * to help distinguish it from other patterns matched.
 */
JSON_WS			[ \t\r]+
JSON_NL			\n+
/*
 * NOTE: On the subject of JSON_STRING one might ask the question about the
 * tighter restrictions on JSON strings and why we don't even consider them.
 * This is a good question but the answer is simple: the JSON string conversion
 * routines actually do these checks. This simplifies the parser regex and so we
 * don't have to worry about complicating the parser unnecessarily.
 */
JSON_STRING		\"([^"\x01-\x1f]|\\\")*\"
JSON_INTEGER		-?([1-9][0-9]*|0)
JSON_FRACTION		"."[0-9]+
JSON_EXPONENT		[Ee][-+]?[0-9]+
JSON_NUMBER		({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT})
JSON_TRUE		"true"
JSON_FALSE		"false"
JSON_NULL		"null"
JSON_OPEN_BRACE		"{"
JSON_CLOSE_BRACE	"}"
JSON_OPEN_BRACKET	"["
JSON_CLOSE_BRACKET	"]"
JSON_COLON		":"
JSON_COMMA		","

/* Actions. */
%%
{JSON_WS}		{
			    /*
			     * Whitespace excluding newlines
			     *
			     * NOTE: We have to include this action as otherwise
			     * the action '.' will return an invalid token to
			     * the parser.
			     *
			     * We don't need the below message but for debugging
			     * purposes we have it printed for now.
			     */
			    (void) json_dbg(JSON_DBG_VVHIGH, __func__, "\nignoring %ju whitespace%s\n",
							     (uintmax_t)yyleng, yyleng==1?"":"s");
			}

{JSON_NL}		{
			    yycolumn = 1;
			}

{JSON_STRING}		{
			    /* string */
			    return JSON_STRING;
			}

{JSON_NUMBER}		{
			    /* number */
			    return JSON_NUMBER;
			}

{JSON_NULL}		{
			    /* null object */
			    return JSON_NULL;
			}

{JSON_TRUE}		{
			    /* boolean: true */
			    return JSON_TRUE;
			}
{JSON_FALSE}		{
			    /* boolean: false */
			    return JSON_FALSE;
			}

{JSON_OPEN_BRACE}	{
			    /* start of object */
			    return JSON_OPEN_BRACE;
			}
{JSON_CLOSE_BRACE}	{
			    /* end of object */
			    return JSON_CLOSE_BRACE;
			}

{JSON_OPEN_BRACKET}	{
			    /* start of array */
			    return JSON_OPEN_BRACKET;
			}
{JSON_CLOSE_BRACKET}	{
			    /* end of array */
			    return JSON_CLOSE_BRACKET;
			}

{JSON_COLON}		{
			    /* colon or 'equals' */
			    return JSON_COLON;
			}

{JSON_COMMA}		{
			    /* comma: name/value pair separator */
			    return JSON_COMMA;
			}

.			{
			    /* invalid token: any other character */
			    warn(__func__, "invalid token: 0x%02x = <%c>", *yytext, *yytext);

			    /*
			     * This is a hack for better error messages with
			     * invalid tokens. Bison syntax error messages are
			     * in the form of:
			     *
			     *	    syntax error, unexpected <token name>
			     *	    syntax error, unexpected <token name>, expecting } or JSON_STRING
			     *
			     * etc. where <token name> is whatever we return
			     * here in the lexer actions (e.g.  JSON_STRING or
			     * in this case literally token) to the parser. But
			     * the problem is what do we call an invalid token
			     * without knowing what what the token actually is?
			     * Thus we call it token so that it will read
			     * literally as 'unexpected token' which removes any
			     * ambiguity (it could otherwise be read as 'it's
			     * unexpected in this place but it is valid in other
			     * contexts' but it's never actually valid: it's a
			     * catch all for anything that's not valid).
			     *
			     * We also make use of yytext in yyerror()
			     * which makes for a somewhat reasonable error
			     * message even when yyerror() is called from
			     * memory exhaustion. See that function and the
			     * comments for '%token token' in jparse.y.
			     */
			    return token;
			}

%%

/* Section 3: Code that's copied to the generated scanner */


/*
 * nul_scan - detect NUL bytes
 *
 * By NUL, we refer to a byte with the value of 0.
 *
 * A flex programmer's apology:
 *
 * Whenever a scanner matches a token, the text of the token is stored in
 * the NUL byte terminated string yytext.  The length in yyleng is the same
 * as the value that would be returned by the strlen(3) function.
 * Even though regular expressions can detect the presence of a NUL bytes
 * in a block of data, error reporting and internal pointer advancement
 * appears to assume NUL terminated strings AND thus the state machine
 * can become "confused" when scanning a block of data with NUL bytes inside
 * instead of just terminating it.
 *
 * To avoid such problems, we pre-scan the block of data for NUL bytes
 * and report their presence if we find any NUL bytes in data.
 *
 * We scan data for both NUL bytes.  When a NUL byte is detected, count
 * we increment the num_errors external variable.
 *
 * We also keep track of newlines.  For the first MAX_NUL_REPORTED occurrences
 * of a NUL byte, we use the werr() function to issue an error reporting
 * both the line number a byte position of the NUL byte.  Once we reach a
 * total of MAX_NUL_REPORTED NUL bytes detected, we issue a "too many NUL"
 * style message and stop reporting further NUL byte error messages.
 * This is done in case we encounter a large block of NUL bytes so as to
 * not flood the system with lots of error messages.
 *
 * We also return false if data == NULL or len <= 0.
 *
 * given:
 *
 *	data	    - pointer to block of data to scan for NUL bytes
 *	len	    - length of data in bytes
 *
 * return:
 *	true ==> data != NULL AND len > 0 AND no NUL bytes found
 *	false ==> data == NULL OR len <= 0 OR one or more NUL bytes found
 */
static bool
nul_scan(char const *data, size_t len)
{
    size_t linenum = 0;	    /* number of newline terminated lines we have processed */
    size_t nul_count = 0;   /* number of NUL bytes detected */
    size_t byte_pos = 0;    /* byte position within the current line number */
    bool report_limit = false;	/* true ==> we have reached the MAX_NUL_REPORTED limit */
    size_t i = 0;

    /*
     * firewall
     */
    if (data == NULL) {
	werr(30, __func__, "data is NULL");
	++num_errors;
	return false;
    }
    if (len <= 0) {
	werr(31, __func__, "len: %zu <= 0 ", len);
	++num_errors;
	return false;
    }

    /*
     * start with the 1st line number
     */
    ++linenum;

    /*
     * scan the data
     */
    for (i=0; i < len; ++i) {

	/* count this byte within the line */
	++byte_pos;

	/*
	 * process the current data byte
	 */
        switch (data[i]) {
	case '\0':	/* NUL byte */

	    /* count NUL byte */
	    ++nul_count;

	    /*
	     * case: below the report limit
	     */
	    if (nul_count <= MAX_NUL_REPORTED) {
		werr(32, __func__, "invalid NUL (0) byte detected in line: %zu byte position: %zu",
			 linenum, byte_pos);

	    /*
	     * case: at or above the report limit
	     */
	    } else if (report_limit == false) {
		werr(33, __func__, "too many NUL (0) bytes detected: additional NUL byte reports disabled");
		report_limit = true;	/* report reaching report limit only once */
	    }
	    break;

	case '\n':	/* newline */

	    /* count newline */
	    ++linenum;
	    byte_pos = 0;	/* reset byte position */
	    break;

	default:	/* normal data byte */
	    break;
	}
    }

    /*
     * return the NUL detection status
     */
    if (nul_count > 0) {
	return true;
    }
    return false;
}


/*
 * parse_json - parse a JSON document of a given length
 *
 * Given a pointer to char and a length, use the parser to determine if the json
 * is valid or not.
 *
 * given:
 *
 *	ptr	    - pointer to start of json blob
 *	len	    - length of the json blob
 *	is_valid    - != NULL ==> set to true or false depending on json validity
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * NOTE: The reason this is in the scanner and not the parser is because
 * YY_BUFFER_STATE is part of the scanner and not the parser.
 *
 * NOTE: This function only warns on error. This is so that an entire report of
 * all the problems can be given at the end if the verbosity level is high
 * enough (or otherwise if this information is requested).
 */
struct json *
parse_json(char const *ptr, size_t len, bool *is_valid)
{
    struct json *tree = NULL;		/* the JSON parse tree */
    int ret = 0;			/* yyparse() return value */

    /*
     * We set *is_valid = true if is_valid != NULL so that the caller does not
     * have to worry about this for each call. That way when the function
     * returns they can check their bool and know whether it was valid or not
     * (though we actually report invalid json too in case but the caller has
     * information about the filename when passed a file and can now report if
     * the json was valid).
     *
     * Either way this ensures that the caller has a valid value after this
     * function returns (as long as is_valid != NULL).
     */
    if (is_valid != NULL) {
	*is_valid = true;
    }

    /*
     * firewall
     */
    if (ptr == NULL) {

	/* this should never happen */
	werr(34, __func__, "ptr is NULL");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	tree = json_alloc(JTYPE_UNSET);
	return tree;
    }

    /*
     * scan the blob
     */
    yylineno = 1;
    bs = yy_scan_bytes(ptr, len);
    if (bs == NULL) {
	/*
	 * if unable to scan the bytes it indicates an internal error and
	 * perhaps it should call err() instead but for now we make it a
	 * non-fatal error as well.
	 */
	werr(35, __func__, "unable to scan string");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	tree = json_alloc(JTYPE_UNSET);
	return tree;
    }

    /*
     * announce beginning to parse
     */
    if (json_dbg_allowed(JSON_DBG_VVHIGH)) {
	fprstr(stderr, "*** BEGIN PARSE\n");
	fprstr(stderr, "<\n");
	(void) fprint_line_buf(stderr, (void *)ptr, len, 0, 0);
	fprstr(stderr, "\n>\n");
    }

    /*
     * parse the blob, passing into the parser the node
     */
    ret = yyparse(&tree);

    /*
     * scan and parse clean up
     */
    yy_delete_buffer(bs);
    bs = NULL;

    /*
     * announce end of parse
     */
    if (json_dbg_allowed(JSON_DBG_VVHIGH)) {
	fprstr(stderr, "*** END PARSE\n");
    }

    /*
     * report scanner / parser success or failure
     */
    if (json_dbg_allowed(JSON_DBG_VVHIGH)) {
	json_dbg(JSON_DBG_VVHIGH, __func__, "yyparse() returned: %d", ret);
    }
    if (ret == 0) {
	json_dbg(JSON_DBG_LOW, __func__, "valid JSON");
	if (is_valid != NULL) {
	    *is_valid = true;
	}
    } else {
	json_dbg(JSON_DBG_LOW, __func__, "invalid JSON");
	if (is_valid != NULL) {
	    *is_valid = false;
	}
    }

    /*
     * return parse tree
     */
    return tree;
}


/*
 * parse_json_stream - parse an open file stream for JSON data
 *
 * Given an open file stream, we will read in all data (until EOF) and
 * then parse that data as if it were JSON.
 *
 * given:
 *	stream      - open file stream containing JSON data
 *	is_valid    - != NULL ==> set to true or false depending on json validity
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * If stream is NULL or stream is not open, or if read_all() fails,
 * then this function warns and sets *is_valid (if is_valid != NULL) to false.
 *
 * NOTE: The reason this is in the scanner and not the parser is because
 *	 YY_BUFFER_STATE is part of the scanner and not the parser and that's required
 *	 for the parse_json_block() function.
 *
 * NOTE: This function only warns on error. It does this via the called function
 *	 parse_json(). This is done so that an entire report of all the problems can
 *	 be given at the end if the verbosity level is high enough (or otherwise if
 *	 this information is requested).
 */
struct json *
parse_json_stream(FILE *stream, bool *is_valid)
{
    struct json *node = NULL;		/* the JSON parse tree */
    char *data = NULL;			/* used to determine if there are NUL bytes in the file */
    size_t len = 0;			/* length of data read */
    bool nul_detected = false;		/* true ==> a NUL byte was detected in data */

    /*
     * firewall
     */
    if (stream == NULL) {

	/* report NULL stream */
	werr(36, __func__, "stream is NULL");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }
    if (is_open_stream(stream) == false) {

	/* report closed stream */
	werr(37, __func__, "stream is not open");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * yyin becomes our sanity checked open stream for the sake of the parser / scanner
     */
    yyin = stream;

    /*
     * read the entire file into memory
     */
    data = read_all(yyin, &len);
    if (data == NULL) {

	/* warn about read error */
	werr(38, __func__, "could not read read stream");
	++num_errors;
	clearerr_or_fclose(yyin);

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * pre-scan data for NUL bytes
     */
    nul_detected = nul_scan(data, len);
    if (nul_detected == true) {

	/* report invalid JSON */
	werr(39, __func__, "invalid NUL (0) detected: data block is NOT valid JSON");

	/* clean up input stream */
	clearerr_or_fclose(yyin);

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* free data */
	if (data != NULL) {
	    free(data);
	    data = NULL;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * JSON parse the data from the file
     */
    json_dbg(JSON_DBG_HIGH, __func__, "calling parse_json on data block with length %ju:", (uintmax_t)len);
    node = parse_json(data, len, is_valid);

    /* free data */
    if (data != NULL) {
	free(data);
	data = NULL;
    }

    /*
     * clear error or close stream
     *
     * NOTE: clearerr_or_fclose() does nothing if yyin is NULL.
     */
    clearerr_or_fclose(yyin);
    yyin = NULL;

    /*
     * return the JSON parse tree node
     */
    return node;
}


/*
 * parse_json_file	    - parses file file
 *
 * given:
 *	filename    - filename to parse
 *	is_valid    - != NULL ==> set to true or false depending on json validity
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * If filename is NULL or the filename is not a readable file (or is empty) or
 * if read_all() read_all() fails,
 * then this function warns and sets *is_valid (if is_valid != NULL) to false.
 *
 * NOTE: The reason this is in the scanner and not the parser is because
 *	 YY_BUFFER_STATE is part of the scanner and not the parser and that's required
 *	 for the parse_json_block() function.
 *
 * NOTE: This function only warns on error. It does this via the called function
 *	 parse_json(). This is done so that an entire report of all the problems can
 *	 be given at the end if the verbosity level is high enough (or otherwise if
 *	 this information is requested).
 */
struct json *
parse_json_file(char const *filename, bool *is_valid)
{
    struct json *node = NULL;		/* the JSON parse tree */

    /*
     * firewall
     */
    if (filename == NULL) {

	/* this should actually never happen if called from jparse */
	werr(40, __func__, "passed NULL filename");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }
    if (*filename == '\0') { /* strlen(filename) == 0 */

	/* warn about bogus filename */
	werr(41, __func__, "passed empty filename");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * if file is -, then we will parse stdin
     */
    if (strcmp(filename, "-") == 0) {
	yyin = stdin;

    /*
     * case: will read from a file
     */
    } else {

	/*
	 * validate filename
	 */
	if (!exists(filename)) {
	    /* report missing file */
	    werr(42, __func__, "passed filename that's not actually a file: %s", filename);
	    ++num_errors;

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;

	}
	if (!is_file(filename)) {
	    /* report that file is not a normal file */
	    werr(43, __func__, "passed filename not a normal file: %s", filename);
	    ++num_errors;

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;
	}
	if (!is_read(filename)) {

	    /* report unreadable file */
	    werr(44, __func__, "passed filename not a readable file: %s", filename);
	    ++num_errors;

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;
	}

	/*
	 * open file for scanner to use
	 */
	errno = 0;
	yyin = fopen(filename, "r");
	if (yyin == NULL) {

	    /* warn about file open error */
	    werrp(45, __func__, "couldn't open file %s, ignoring", filename);
	    ++num_errors;

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;
	}
    }

    /*
     * JSON parse the open stream
     */
    node = parse_json_stream(yyin, is_valid);

    /*
     * return the JSON parse tree node
     */
    return node;
}
