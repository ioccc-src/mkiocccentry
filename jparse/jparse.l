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
%option noyywrap
%option nodefault nounput noinput
%option yylineno 8bit
%option bison-bridge bison-locations reentrant
%option header-file="jparse.lex.h"
%option extra-type="struct json_extra *"

%{
/* Declarations etc. go here.
 *
 * Code is copied verbatim near the top of the generated code.
 */


/*
 * jparse - JSON parser
 */
#include "jparse.h"


static YY_BUFFER_STATE bs;

/*
 * locations in the file / json block
 */
#define YY_USER_ACTION \
			yylloc->filename = yyextra != NULL ? yyextra->filename:""; \
			yylloc->first_line = yylloc->last_line = yylineno; \
			yylloc->first_column = yyget_column(yyscanner); \
			yylloc->last_column = yyget_column(yyscanner)+yyget_leng(yyscanner)-1; \
			yyset_column(yyget_leng(yyscanner), yyscanner);
%}

/*
 * Section 2: Patterns (regular expressions) and actions.
 */

JSON_WS			[ \t\r]+
JSON_NL			\n+
/*
 * NOTE: On the subject of JSON_STRING one might ask the question about the
 * tighter restrictions on JSON strings and why we don't even consider them.
 * This is a good question but the answer is simple: the JSON string conversion
 * routines actually do these checks. This simplifies the lexer regex and so we
 * don't have to worry about complicating the lexer or parser unnecessarily.
 *
 * We do exclude specific lower bytes but we also pre-scan for these same bytes
 * and if they're there and unescaped we warn about it. The reason to still
 * exclude them is because this way they are not passed to the parser. This
 * might not be necessary because if we find invalid bytes it is an error and
 * the parser will never find them but this is more defensive. For the
 * interested, the old regexp was:
 *
 *	    \"([^\n"]|\\\")*\"
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
			     * purposes we print how many whitespaces are
			     * ignored, if the JSON debug level is high enough.
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
			    warn(__func__, "at line %d column %d: invalid token: 0x%02x = <%c>", yylloc->first_line, yylloc->first_column, *yytext, *yytext);

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
 * low_byte_scan - detect certain low byte values
 *
 * A scan data for bytes in the class: [\x00-\x08\x0e-\x1f]
 *
 * A flex programmer's apology:
 *
 * Whenever a scanner matches a token, the text of the token is stored in
 * the NUL byte terminated string yytext.  The length in yyleng is the same
 * as the value that would be returned by the strlen(3) function.
 * Even though regular expressions can detect the presence of a NUL (\x00) bytes
 * in a block of data, error reporting and internal pointer advancement
 * appears to assume NUL (\x00) terminated strings AND thus the state machine
 * can become "confused" when scanning a block of data with NUL bytes inside
 * instead of just terminating it.
 *
 * To avoid such problems, we pre-scan the block of data for NUL (\x00) bytes
 * and report their presence if we find any NUL (\x00) bytes in data.
 *
 * The so-called JSON spec does not allow byte values [\x00-\x1f] within
 * JSON strings.  Attempts to modify JSON_STRING to exclude such byte values
 * within a JSON string has not worked well, especially when it comes to
 * printing error messages with strings that contain such bytes.
 *
 * To make it easier for the scanner and error messages, we pre-scan data
 * for bytes in the class: [\x00-\x08\x0e-\x1f].  Compared to [\x00-\x1f],
 * this class allows the following subset of whitespace characters:
 *
 *	\x09	Horizontal Tab (\t)
 *	\x0a	Line Feed (\n)
 *	\x0b	Vertical Tab (\v)
 *	\x0c	Form Feed (\f)
 *	\x0d	Carriage Return (\r)
 *
 *	NOTE: Along with \x20 (space) this represents the ASCII whitespace set.
 *	      We avoid scanning for \x20 (space) by limiting our upper exclusion
 *	      range to \x1f.
 *
 * While this subset of whitespace characters are strictly not allowed in
 * JSON strings, we won't scan for them because it might be reasonable for
 * those characters to used outside elements of JSON syntax:
 *
 * We scan data for bytes in the class: [\x00-\x08\x0e-\x1f].  When such
 * bytes are detected, we increment the external variable, we will indicate to
 * the caller that the JSON is invalid by returning true.
 *
 * We also keep track of newlines.  Not to exclude them, but to make it easy
 * for this routine to reference line number and byte position in the line.
 *
 * When a byte in the class: [\x00-\x08\x0e-\x1f] is detected we consider
 * issuing a warning message that indicates the line number, byte position in
 * the line, and byte value.  To avoid printing a huge flood of warning messages
 * (say should a large block of NUL (\x00) and SOM (\x01) bytes are
 * encountered), we limit the number of NUL (\x00) byte warnings to the first
 * MAX_NUL_REPORTED, and limit the number of low ([\x01-\x08\x0e-\x1f]) byte
 * warnings to the first MAX_LOW_REPORTED.  Should we find more warnings that
 * the limit, a one time "too many" warning is issues.
 *
 * given:
 *
 *	data	    - pointer to block of data to scan for NUL bytes
 *	len	    - length of data in bytes
 *	low_bytes   - pointer to set the number of low bytes found (that aren't NUL) in
 *	nul_bytes   - pointer to set the number of NUL bytes found in
 *
 * return:
 *	true ==> data == NULL  OR  len <= 0 OR  one or more [\x00-\x08\x0e-\x1f]
 *		 bytes are found OR low_bytes == NULL OR nul_bytes == NULL
 *	false ==> data != NULL AND len > 0  AND no NUL [\x00-\x08\x0e-\x1f] bytes were found
 */
static bool
low_byte_scan(char const *data, size_t len, size_t *low_bytes, size_t *nul_bytes)
{
    size_t linenum = 0;	    /* number of newline terminated lines we have processed */
    size_t byte_pos = 0;    /* byte position within the current line number */
    bool nul_limit = false;	/* true ==> found too many NUL (\x00) bytes to report all */
    bool low_limit = false;	/* true ==> found too many low ([\x01-\x08\x0e-\x1f]) bytes to report all */
    size_t i = 0;

    /*
     * firewall
     */
    if (data == NULL) {
	werr(30, __func__, "data is NULL");
	return true;
    }
    if (len <= 0) {
	werr(31, __func__, "len: %zu <= 0 ", len);
	return true;
    }
    if (low_bytes == NULL) {
	werr(32, __func__, "low_bytes is NULL");
	return true;
    } else {
	/* set *low_bytes to 0 */
	*low_bytes = 0;
    }
    if (nul_bytes == NULL) {
	werr(33, __func__, "nul_bytes is NULL"); /* nul_bytes is ironically NULL! :-) */
	return true;
    } else {
	*nul_bytes = 0;
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
	    ++(*nul_bytes);

	    /*
	     * case below the report limit
	     */
	    if (*nul_bytes <= MAX_NUL_REPORTED) {
		werr(34, __func__, "invalid NUL (0) byte detected in line: %zu byte position: %zu",
			 linenum, byte_pos);

	    /*
	     * case at or above the report limit
	     */
	    } else if (nul_limit == false) {
		werr(35, __func__, "too many NUL ('\\0') bytes detected: "
			 "additional NUL byte reports disabled");
		nul_limit = true;	/* report reaching report limit only once */
	    }
	    break;

	/*
	 * For grins, we document some of the historical byte meanings :-)
	 */
	case '\x01':	/* SOH - Start of Heading */
	case '\x02':	/* EOA - Start of Text */
	case '\x03':	/* EOM - End of Text */
	case '\x04':	/* EOT - End of Transmission */
	case '\x05':	/* ENQ - Enquiry */
	case '\x06':	/* ACK - Acknowledgement */
	case '\x07':	/* BEL - Bell */
	case '\x08':	/* BS - Backspace */
	case '\x0e':	/* SO - Shift Out */
	case '\x0f':	/* SI - Shift In */
	case '\x10':	/* DCO / DLE - Data Link Escape */
	case '\x11':	/* DC1 / XON - Device Control 1 */
	case '\x12':	/* DC2 - Device Control 2 */
	case '\x13':	/* DC3 / XOFF - Device Control 3 */
	case '\x14':	/* DC4 - Device Control 4 */
	case '\x15':	/* ERR / NAK - Negative Acknowledgement */
	case '\x16':	/* SYNC / SYN - Synchronous Idle */
	case '\x17':	/* LEM / ETB - End of Transmission Block */
	case '\x18':	/* S0 / CAN - Cancel */
	case '\x19':	/* S1 / EM - End of Medium */
	case '\x1a':	/* S2 / SS / SUB - Substitute */
	case '\x1b':	/* S3 / ESC - Escape */
	case '\x1c':	/* S4 / FS - File Separator */
	case '\x1d':	/* S5 / GS - Group Separator */
	case '\x1e':	/* S6 / RS - Record Separator */
	case '\x1f':	/* S7 / US - Unit Separator */

	    /* count low ([\x01-\x08\x0e-\x1f]) byte */
	    ++(*low_bytes);

	    /*
	     * case below the report limit
	     */
	    if (*low_bytes <= MAX_LOW_REPORTED) {
		werr(36, __func__, "invalid LOW byte 0x%02x detected in line: %zu byte position: %zu",
			 data[i], linenum, byte_pos);

	    /*
	     * case at or above the report limit
	     */
	    } else if (low_limit == false) {
		werr(37, __func__, "too many LOW bytes ([\\x01-\\x08\\x0e-\\x1f] detected: "
		         "additional low byte reports disabled");
		low_limit = true;	/* report reaching report limit only once */
	    }
	    break;

	case '\n':	/* \x0a - LF - Line Feed / Newline */

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
    if (*nul_bytes > 0 || *low_bytes > 0) {
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
 *	filename    - filename or NULL for stdin
 *	is_valid    - set to true or false depending on json validity (must NOT
 *		      be NULL)
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
parse_json(char const *ptr, size_t len, char const *filename, bool *is_valid)
{
    struct json *tree = NULL;		/* the JSON parse tree */
    int ret = 0;			/* yyparse() return value */
    yyscan_t scanner;			/* scanner instance: is a void * */
    struct json_extra extra;

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
    } else {
	/* even so it's an error if is_valid is NULL */
	err(38, __func__, "is_valid == NULL");
	not_reached();
    }

    /*
     * firewall
     */
    if (ptr == NULL) {

	/* this should never happen */
	werr(39, __func__, "ptr is NULL");

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	tree = json_alloc(JTYPE_UNSET);
	return tree;
    }

    if (filename == NULL) {
	json_dbg(JSON_DBG_HIGH, __func__, "filename is NULL, forcing it to be \"-\" for stdin");
	filename = "-";	/* assume stdin */
    }

    /*
     * initialise scanner
     */
    extra.filename = filename;
    yylex_init_extra(&extra, &scanner);

    /*
     * scan the blob
     */
    bs = yy_scan_bytes(ptr, (int)len, scanner);
    if (bs == NULL) {
	/*
	 * if unable to scan the bytes it indicates an internal error and
	 * perhaps it should call err() instead but for now we make it a
	 * non-fatal error as well.
	 */
	werr(40, __func__, "unable to scan string");

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	tree = json_alloc(JTYPE_UNSET);
	yylex_destroy(scanner);
	return tree;
    }

    /*
     * we cannot set the column and probably line number without first having a
     * buffer which is why the yy_scan_bytes() is called first.
     */
    yyset_column(1, scanner);
    yyset_lineno(1, scanner);

    /*
     * announce beginning to parse
     */
    if (json_dbg_allowed(JSON_DBG_VVHIGH)) {
	fprstr(stderr, "*** BEGIN PARSE\n");
	fprstr(stderr, "<\n");
	(void) fprint_line_buf(stderr, ptr, len, 0, 0);
	fprstr(stderr, "\n>\n");
    }

    /*
     * parse the blob, passing into the parser the node and scanner
     */
    ret = yyparse(&tree, scanner);

    /*
     * free memory associated with bytes scanned by yy_scan_bytes()
     */
    yy_delete_buffer(bs, scanner);
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

    /* free scanner */
    yylex_destroy(scanner);

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
 *	filename    - name of file or NULL for stdin
 *	is_valid    - set to true or false depending on json validity (must NOT
 *		      be NULL)
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * If stream is NULL or stream is not open, or if read_all() fails,
 * then this function warns and sets *is_valid to false.
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
parse_json_stream(FILE *stream, char const *filename, bool *is_valid)
{
    struct json *node = NULL;		/* the JSON parse tree */
    char *data = NULL;			/* used to determine if there are NUL bytes in the file */
    size_t len = 0;			/* length of data read */
    bool low_byte_detected = false;	/* true ==> byte values [\x00-\x08\x0e-\x1f] detected in data */
    size_t nul_bytes = 0;		/* number of NUL bytes found */
    size_t low_bytes = 0;		/* number of low bytes that are not NUL found */

    /*
     * firewall
     */
    if (is_valid == NULL) {
	err(41, __func__, "is_valid == NULL");
	not_reached();
    } else {
	/*
	 * set to true so that the caller does not need to worry about it (well
	 * they should in case this is removed but we do it explicitly for them
	 * anyway).
	 */
	*is_valid = true;
    }

    if (stream == NULL) {

	/* report NULL stream */
	werr(42, __func__, "stream is NULL");

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
	werr(43, __func__, "stream is not open");

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    if (filename == NULL) {
	json_dbg(JSON_DBG_HIGH, __func__, "filename is NULL, forcing it to be \"-\" for stdin");
	filename = "-";
    }

    /*
     * read the entire file into memory
     */
    data = read_all(stream, &len);
    if (data == NULL) {

	/* warn about read error */
	werr(44, __func__, "could not read read stream");
	clearerr_or_fclose(stream);

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }

    /*
     * pre-scan data for byte values from [\x00-\x08\x0e-\x1f]
     */
    low_byte_detected = low_byte_scan(data, len, &low_bytes, &nul_bytes);
    if (low_byte_detected || low_bytes > 0 || nul_bytes > 0) {

	/* report invalid JSON */
	if (low_bytes > 0 && nul_bytes > 0) {
	    werr(45, __func__, "%ju low byte%s and %ju NUL byte%s detected: data block is NOT valid JSON",
		    (uintmax_t)low_bytes, low_bytes > 1 ? "s":"",
		    (uintmax_t)nul_bytes, nul_bytes > 1 ? "s":"");
	} else if (low_bytes > 0) {
	    werr(46, __func__, "%ju low byte%s detected: data block is NOT valid JSON",
		    (uintmax_t)low_bytes, low_bytes > 1 ? "s":"");
	} else if (nul_bytes > 0) {
	    werr(47, __func__, "%ju NUL byte%s detected: data block is NOT valid JSON",
		    (uintmax_t)nul_bytes, nul_bytes > 1 ? "s":"");
	}

	/* clean up input stream */
	clearerr_or_fclose(stream);

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
    node = parse_json(data, len, filename, is_valid);

    /* free data */
    if (data != NULL) {
	free(data);
	data = NULL;
    }

    /*
     * clear error or close stream
     *
     * NOTE: clearerr_or_fclose() does nothing if stream is NULL.
     */
    clearerr_or_fclose(stream);
    stream = NULL;

    /*
     * return the JSON parse tree node
     */
    return node;
}


/*
 * parse_json_file	    - parses file file
 *
 * given:
 *	name	    - filename of file to parse
 *	is_valid    - set to true or false depending on json validity (must NOT
 *		      be NULL)
 *
 * return:
 *	pointer to a JSON parse tree
 *
 * If name is NULL or the name is not a readable file (or is empty) or
 * if read_all() read_all() fails, then this function warns and sets *is_valid
 * to false.
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
parse_json_file(char const *name, bool *is_valid)
{
    struct json *node = NULL;		/* the JSON parse tree */
    FILE *stream = NULL;		/* file stream to read from */

    /*
     * firewall
     */
    if (is_valid == NULL) {
	err(48, __func__, "is_valid == NULL");
	not_reached();
    } else {
	/* assume valid JSON to start out */
	*is_valid = true;
    }
    if (name == NULL) {

	/* this should actually never happen if called from jparse */
	werr(49, __func__, "passed NULL name");

	/* if allowed, report invalid JSON */
	if (is_valid != NULL) {
	    *is_valid = false;
	}

	/* return a blank JSON tree */
	node = json_alloc(JTYPE_UNSET);
	return node;
    }
    if (*name == '\0') { /* strlen(name) == 0 */

	/* warn about bogus name */
	werr(50, __func__, "passed empty filename");

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
    if (strcmp(name, "-") == 0) {
	stream = stdin;

    /*
     * case will read from a file
     */
    } else {

	/*
	 * validate filename
	 */
	if (!exists(name)) {
	    /* report missing file */
	    werr(51, __func__, "passed filename that's not actually a file: %s", name);

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;

	}
	if (!is_file(name)) {
	    /* report that file is not a normal file */
	    werr(52, __func__, "passed filename not a normal file: %s", name);

	    /* if allowed, report invalid JSON */
	    if (is_valid != NULL) {
		*is_valid = false;
	    }

	    /* return a blank JSON tree */
	    node = json_alloc(JTYPE_UNSET);
	    return node;
	}
	if (!is_read(name)) {

	    /* report unreadable file */
	    werr(53, __func__, "passed filename not a readable file: %s", name);

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
	stream = fopen(name, "r");
	if (stream == NULL) {

	    /* warn about file open error */
	    werrp(54, __func__, "couldn't open file %s, ignoring", name);

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
    node = parse_json_stream(stream, name, is_valid);

    /*
     * return the JSON parse tree node
     */
    return node;
}
