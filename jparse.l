/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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

/*
 * An IOCCC satirical take on flex and bison
 *
 * As we utterly object to the hideous code that flex and bison generate we
 * point it out in an ironic way by changing the prefix yy to sorry_ so that
 * flex actually calls itself sorry. This is satire for the IOCCC (although we
 * still believe that flex generates sorry code)!
 *
 * This means that for some symbols later in the file we use sorry_ instead of
 * yy. For example it's sorry_lval instead of yylval and best of all sorry_parse()
 * for it _is_ an sorry parser. Another one is sorry_error() instead of yyerror().
 *
 * A negative consequence here is that because of the prefix being set to
 * sorry_ there's a typedef in the parser that _might_ suggest _our_ struct json
 * is sorry:
 *
 *	typedef struct json SORRY_STYPE;
 *
 * At first glance this is a valid concern. However we argue that even if this
 * is so the struct might well have to be sorry because it's for a json parser; a
 * json parser necessarily has to maintain the mis-features of spec being implemented.
 *
 * BTW: If you want to see all the symbols (re?)defined to something sorry run:
 *
 *	grep -i '#[[:space:]]*define[[:space:]].*sorry_' *.c
 *
 * after generating the files; and if you want to see only what was changed from
 * yy or YY to refer to sorry_ or SORRY_:
 *
 *	grep -i '#[[:space:]]*define[[:space:]]*yy.*sorry_' *.c
 *
 * This will help you find the right symbols should you need them. If (as is
 * likely to happen) the parser is split into another repo for a json parser by
 * itself I will possibly remove this prefix: this is as satire for the IOCCC
 * (though we all believe that the generated code is in fact sorry).
 *
 * WARNING: Although we use the prefix sorry_ the scanner and parser will at
 * times refer to yy and YY and other times refer to sorry_ and SORRY_ (partly
 * because WE refer to sorry_ and SORRY_). So if you're trying to sift through
 * that sorry spaghetti code (which we strongly recommend you do not do as it will
 * likely cause nightmares and massive brain pain) you'll want to check yy/YY as
 * well as sorry_/SORRY_. But really you oughtn't try and go through that code so
 * you need only pay attention to the sorry_ and SORRY_ prefixes (in the *.l and
 * *.y files) which again are satire for the IOCCC. See also the apology in the
 * generated files or directly looking at sorry.tm.ca.h.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */
%option prefix="sorry_"

%{
/* Declarations etc. go here.
 *
 * Code is copied verbatim near the top of the generated code.
 */


/*
 * jparse - JSON parser demo tool
 */
#include "jparse.h"


/*
 * An exception where the prefix does not change YY to SORRY_ is YY_BUFFER_STATE
 * but because it IS sorry we have done it for them in jparse.h so that where one
 * sees SORRY__BUFFER_STATE it's actually YY_BUFFER_STATE.
 */
SORRY__BUFFER_STATE bs;
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
JSON_WS			[ \t\r\n]+

/*
 * NOTE: On the subject of JSON_STRING one might ask the question about the
 * tighter restrictions on JSON strings and why we don't even consider them.
 * This is a good question but the answer is simple: the JSON string conversion
 * routines actually do these checks. This simplifies the parser regex and so we
 * don't have to worry about complicating the parser unnecessarily.
 */
JSON_STRING		\"([^\n"]|\\\")*\"
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
			     * Whitespace
			     *
			     * NOTE: We have to include this action as otherwise
			     * the action '.' will return an invalid token to
			     * the parser.
			     *
			     * We don't need the below message but for debugging
			     * purposes we have it printed for now.
			     */
			    (void) json_dbg(JSON_DBG_VVHIGH, __func__, "\nignoring %ju whitespace%s\n",
							     (uintmax_t)sorry_leng, yyleng==1?"":"s");
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
			    warn(__func__, "\ninvalid token: 0x%02x = <%c>", *sorry_text, *sorry_text);

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
			     * ambiguity (it could be read as 'it's unexpected
			     * in this place but it is valid in other contexts'
			     * but it's never actually valid: it's a catch all
			     * for anything that's not valid.
			     *
			     * We also make use of sorry_text in sorry_error()
			     * which makes for a somewhat reasonable error
			     * message even when sorry_error() is called from
			     * memory exhaustion. See that function and the
			     * comments for '%token token' in jparse.y.
			     */
			    return token;
			}

%%

/* Section 3: Code that's copied to the generated scanner */

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
 * SORRY__BUFFER_STATE is part of the scanner and not the parser.
 *
 * NOTE: This function only warns on error. This is so that an entire report of
 * all the problems can be given at the end if the verbosity level is high
 * enough (or otherwise if this information is requested).
 */
struct json *
parse_json(char const *ptr, size_t len, bool *is_valid)
{
    struct json *tree = NULL;		/* the JSON parse tree */
    int ret = 0;			/* sorry_parse() return value */

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
	werr(30, __func__, "ptr is NULL");
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
    sorry_lineno = 1;
    bs = sorry__scan_bytes(ptr, len);
    if (bs == NULL) {
	/*
	 * if unable to scan the bytes it indicates an internal error and
	 * perhaps it should call err() instead but for now we make it a
	 * non-fatal error as well.
	 */
	/* XXX - should this be a fatal error ? - XXX */
	werr(31, __func__, "unable to scan string");
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
    ret = sorry_parse(&tree);

    /*
     * scan and parse clean up
     */
    sorry__delete_buffer(bs);
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
	json_dbg(JSON_DBG_VVHIGH, __func__, "sorry_parse() returned: %d", ret);
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
 * parse_json_file	    - parses file file
 *
 * given:
 *
 *	filename    - filename to parse
 *	is_valid    - != NULL ==> set to true or false depending on json validity
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * If filename is NULL or the filename is not a readable file (or is empty) or
 * if read_all() returns NULL the function warns but does nothing else.
 *
 * NOTE: The reason this is in the scanner and not the parser is because
 * YY_BUFFER_STATE is part of the scanner and not the parser and that's required
 * for the parse_json_block() function so I think both ought to be here.
 *
 * NOTE: This function only warns on error. It does this via the called function
 * parse_json(). This is done so that an entire report of all the problems can
 * be given at the end if the verbosity level is high enough (or otherwise if
 * this information is requested).
 */
struct json *
parse_json_file(char const *filename, bool *is_valid)
{
    struct json *node = NULL;		/* the JSON parse tree */
    bool is_stdin = false;	/* true if reading from stdin (filename == "-") */
    char *data = NULL;		/* used to determine if there are NUL bytes in the file */
    size_t len = 0;		/* length of data read */
    int ret;

    /*
     * firewall
     */
    if (filename == NULL) {

	/* this should actually never happen if called from jparse */
	werr(32, __func__, "passed NULL filename");
	++num_errors;

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;

    } else if (*filename == '\0') { /* strlen(filename) == 0 */

	/* warn about bogus filename */
	werr(33, __func__, "passed empty filename");
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
     * if file is -, then this as a special way to indicate stdin
     */
    if (strcmp(filename, "-") == 0) {
	is_stdin = true;
    }

    /*
     * case: will read from stdin
     */
    if (is_stdin == true) {
	sorry_in = stdin;

    /*
     * case: will read from a file
     */
    } else {

	/*
	 * validate filename
	 */
	if (!exists(filename)) {
	    /* report missing file */
	    werr(34, __func__, "passed filename that's not actually a file: %s", filename);
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
	    werr(35, __func__, "passed filename not a normal file: %s", filename);
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
	    werr(36, __func__, "passed filename not a readable file: %s", filename);
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
	sorry_in = fopen(filename, "r");
	if (sorry_in == NULL) {

	    /* warn about file open error */
	    werrp(37, __func__, "couldn't open file %s, ignoring", filename);
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
     * read the entire file into memory
     */
    data = read_all(sorry_in, &len);
    if (data == NULL) {
	/* warn about read error */
	/* XXX - should this be a fatal error ? - XXX */
	werr(38, __func__, "couldn't read in %s", is_stdin?"stdin":filename);
	++num_errors;
	clearerr_or_fclose(filename, sorry_in);

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * JSON parse the data from the file
     */
    json_dbg(JSON_DBG_HIGH, __func__, "Calling parse_json with length %ju:", (uintmax_t)len);
    node = parse_json(data, len, is_valid);

    /* free data */
    if (data != NULL) {
	free(data);
	data = NULL;
    }

    /*
     * we cannot use clearerr_or_fclose() here as it's possible that sorry_in
     * will be NULL and so we do it manually instead.
     */
    if (is_stdin == true) {
	clearerr(stdin);
    } else if (sorry_in != NULL) {
	errno = 0;
	ret = fclose(sorry_in);
	if (ret != 0) {
	    warnp(__func__, "error in fclose on file %s", filename);
	}
	sorry_in = NULL;
    }

    /*
     * return the JSON parse tree node
     */
    return node;
}
