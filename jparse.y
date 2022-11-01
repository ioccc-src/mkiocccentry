/*
 * JSON parser - bison grammar
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed by:
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


/* Section 1: Declarations */
/*
 * We enable lookahead correction parser for improved errors
 */
%define parse.lac full

/* we want a re-entrant parser */
%define api.pure full

/* we need locations for better error reporting */
%locations

/*
 * We use our struct json (see json_parse.h for its definition) instead of bison
 * %union.
 */
%define api.value.type {struct json *}

/*
 * we need access to the tree in parse_json() so we tell bison that yyparse()
 * takes a struct json **tree.
 */
%parse-param { struct json **tree }

%{



/*
 * jparse - JSON parser demo tool
 */
#include "jparse.h"


unsigned num_errors = 0;		/* > 0 number of errors encountered */

/*
 * bison debug information for development
 */
int yydebug = 0;	/* 0 ==> verbose bison debug off, 1 ==> verbose bison debug on */

%}


/*
 * Terminal symbols (token kind)
 *
 * For most of the terminal symbols we use string literals to identify them as
 * this makes it easier to read error messages. This feature is not POSIX Yacc
 * compatible but we've decided that the benefit outweighs this fact.
 */
%token JSON_OPEN_BRACE "{"
%token JSON_CLOSE_BRACE "}"
%token JSON_OPEN_BRACKET "["
%token JSON_CLOSE_BRACKET "]"
%token JSON_COMMA ","
%token JSON_COLON ":"
%token JSON_NULL "null"
%token JSON_TRUE "true"
%token JSON_FALSE "false"
%token JSON_STRING
%token JSON_NUMBER

/*
 * The next token 'token' is a hack for better error messages with invalid
 * tokens.  Bison syntax error messages are in the form of:
 *
 *	    syntax error, unexpected <token name>
 *	    syntax error, unexpected <token name>, expecting } or JSON_STRING
 *
 * etc. where <token name> is whatever is returned in the lexer actions (e.g.
 * JSON_STRING) to the parser. But the problem is what do we call an invalid
 * token without knowing what what the token actually is? Thus we call it token
 * so that it will read literally as 'unexpected token' which removes any
 * ambiguity (otherwise it could be read as 'it's unexpected in this place but
 * it is valid in other contexts' but it's actually never valid: it's a catch
 * all for anything that's not valid).
 *
 * Then as a hack (or maybe kludge) in yyerror() we refer to yytext in a
 * way that shows what the token is that caused the failure (whether it's a
 * syntax error or something else).
 */
%token token


/*
 * Section 2: Rules
 *
 * See https://www.json.org/json-en.html for the JSON specification. We have
 * tried to make the below grammar as close to the JSON specification as
 * possible for those who are familiar with it but there might be some minor
 * differences in order or naming. One that we do not have is whitespace but
 * that's not needed and would actually cause many complications and parsing
 * errors. There are some others we do not need to include as well.
 */
%%
json:

    json_element
    {
	/*
	 * $$ = $json
	 * $1 = $json_element
	 */

	/* pre action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json: starting: "
					      "json: json_element");
	    json_dbg(JSON_DBG_HIGH, __func__, "under json: $json_element type: %s",
					     json_item_type_name($json_element));
	    json_dbg(JSON_DBG_HIGH, __func__, "under json: about to perform: "
					     "$json = $json_element;");
	}

	/* action */
	$json = $json_element;	/* magic: json_value becomes json_element's type */

	/* post action debugging */
	if (json_dbg_allowed(JSON_DBG_MED)) {
	    if (json_dbg_allowed(JSON_DBG_HIGH)) {
		json_dbg(JSON_DBG_HIGH, __func__, "under json: returning $json_element type: %s",
						  json_item_type_name($json));
	    }
	    json_dbg_tree_print(JSON_DBG_MED, __func__, $json, JSON_DEFAULT_MAX_DEPTH);
	}

	/* return the JSON parse tree */
	if ($json == NULL) {
	    /* Magic: See below magic note for how *tree will be set to NULL */
	    warn(__func__, "under json: $json == NULL: about to perform: "
			   "*tree = NULL;");
	}
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json: about also to perform: "
					      "*tree = $json;");
	}
	if (tree == NULL) {
	    warn(__func__, "under json: tree == NULL");
	} else {
	    /*
	     * Magic: If $json == NULL then *tree will be set to NULL. If it's
	     * not NULL, however, *tree will be set to the parse tree itself
	     * ($json).
	     */
	    *tree = $json;	/* more magic: set yyparse(tree) arg to point to JSON parse tree */
	}
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json: ending: "
					      "json: json_element");
	}
    }
    ;

json_value:

    json_object
    {
	/*
	 * $$ = $json_value
	 * $1 = $json_object
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: json_object");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: $json_object type: %s",
					     json_item_type_name($json_object));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = $json_object;");
	}

	/* action */
	$json_value = $json_object;	/* magic: json_value becomes JTYPE_OBJECT type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: json_object");
	}
    }
    |

    json_array
    {
	/*
	 * $$ = $json_value
	 * $1 = $json_array
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: json_array");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: $json_array type: %s",
					       json_item_type_name($json_array));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = $json_array;");
	}

	/* action */
	$json_value = $json_array;	/* magic: json_value becomes JTYPE_ARRAY type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: json_array");
	}
    }
    |

    json_string
    {
	/*
	 * $$ = $json_value
	 * $1 = $json_string
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: json_string");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: $json_string type: %s",
					       json_item_type_name($json_string));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = $json_string;");
	}

	/* action */
	$json_value = $json_string; /* magic: json_value becomes JTYPE_STRING type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: json_string");
	}
    }
    |

    json_number
    {
	/*
	 * $$ = $json_value
	 * $1 = $json_number
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: json_number");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: $json_number type: %s",
					       json_item_type_name($json_number));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = $json_number;");
	}

	/* action */
	$json_value = $json_number; /* magic: json_value becomes JTYPE_NUMBER type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: json_number");
	}
    }
    |

    JSON_TRUE
    {
	/*
	 * $$ = $json_value
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: JSON_TRUE");
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yytext: <%s>", yytext);
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yyleng: <%d>", yyleng);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = parse_json_bool(yytext);");
	}

	/* action */
	$json_value = parse_json_bool(yytext); /* magic: json_value becomes JTYPE_BOOL type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: JSON_TRUE");
	}
    }
    |

    JSON_FALSE
    {
	/*
	 * $$ = $json_value
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: JSON_FALSE");
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yytext: <%s>", yytext);
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yyleng: <%d>", yyleng);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = parse_json_bool(yytext);");
	}

	/* action */
	$json_value = parse_json_bool(yytext); /* magic: json_value becomes JTYPE_BOOL type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					     "json_value: JSON_FALSE");
	}
    }
    |

    JSON_NULL
    {
	/*
	 * $$ = $json_value
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: starting: "
					       "json_value: JSON_NULL");
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yytext: <%s>", yytext);
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_value: yyleng: <%d>", yyleng);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: about to perform: "
					       "$json_value = parse_json_null(yytext);");
	}

	/* action */
	$json_value = parse_json_null(yytext); /* magic: json_value becomes JTYPE_NULL type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_value: returning $json_value type: %s",
					      json_item_type_name($json_value));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_value, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_value: ending: "
					       "json_value: JSON_NULL");
	}
    }
    ;

json_object:

    JSON_OPEN_BRACE json_members JSON_CLOSE_BRACE
    {
	/*
	 * $$ = $json_object
	 * $1 = $json_members
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: starting: "
					       "json_object: JSON_OPEN_BRACE json_members JSON_CLOSE_BRACE");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: $json_members type: %s",
					       json_item_type_name($json_members));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: about to perform: "
					       "$json_object = $json_members;");
	}

	/* action */
	$json_object = $json_members; /* magic: json_value becomes JTYPE_OBJECT type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_object: returning $json_object type: %s",
					      json_item_type_name($json_object));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_object, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: ending: "
					       "json_object: JSON_OPEN_BRACE json_members JSON_CLOSE_BRACE");
	}
    }
    |

    JSON_OPEN_BRACE JSON_CLOSE_BRACE
    {
	/*
	 * $$ = $json_object
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: starting: "
					       "json_object: JSON_OPEN_BRACE JSON_CLOSE_BRACE");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: about to perform: "
					       "$json_object = json_create_object();");
	}

	/* action */
	$json_object = json_create_object(); /* json_object becomes JTYPE_OBJECT */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_object: returning $json_object type: %s",
					      json_item_type_name($json_object));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_object, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_object: ending: "
					       "json_object: JSON_OPEN_BRACE JSON_CLOSE_BRACE");
	}
    }
    ;

json_members:

    json_member
    {
	/*
	 * $$ = $json_members
	 * $1 = $json_member
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: starting: "
					       "json_members: json_member");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: $json_member type: %s",
					       json_item_type_name($json_member));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: about to perform: "
					       "$json_members = json_create_object();");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: about to also perform: "
					       "$json_members = json_object_add_member($json_members, $json_member);");
	}

	/* action */
	$json_members = json_create_object();
	$json_members = json_object_add_member($json_members, $json_member);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: returning $json_members type: %s",
					       json_item_type_name($json_members));
	    json_dbg_tree_print(JSON_DBG_VHIGH, __func__, $json_members, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_members: ending: "
					      "json_members: json_member");
	}
    }
    |

    json_members JSON_COMMA json_member
    {
	/*
	 * $$ = $json_members
	 * $1 = $json_members
	 * $3 = $json_member
	 *
	 * NOTE: Cannot use $json_members due to ambiguity.
	 *       But we can use $3 for $json_member.
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: starting: "
					       "json_members: json_members JSON_COMMA json_member");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: $1 ($json_members) type: %s",
					       json_item_type_name($1));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: $3 ($json_member) type: %s",
					       json_item_type_name($3));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: about to perform: "
					       "$$ = json_object_add_member($1, $json_member)");
	}

	/* action */
	$$ = json_object_add_member($1, $json_member);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_members: returning $$ ($json_members) type: %s",
					      json_item_type_name($$));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $$, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_members: ending: "
					       "json_members: json_members JSON_COMMA json_member");
	}
    }
    ;

json_member:

    json_string JSON_COLON json_element
    {
	/*
	 * $$ = $json_member
	 * $1 = $json_string
	 * $3 = $json_element
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_member: starting: "
					       "json_member: json_string JSON_COLON json_element");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_member: $json_string type: %s",
					       json_item_type_name($json_string));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_member: $json_element type: %s",
					       json_item_type_name($json_element));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_member: about to perform: "
					       "$json_member = parse_json_member($json_string, $json_element);");
	}

	/* action */
	$json_member = parse_json_member($json_string, $json_element);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_member: returning $json_member type: %s",
					      json_item_type_name($json_member));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_member, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_member: ending: "
					       "json_member: json_string JSON_COLON json_element");
	}
    }
    ;

json_array:

    JSON_OPEN_BRACKET json_elements JSON_CLOSE_BRACKET
    {
	/*
	 * $$ = $json_array
	 * $2 = $json_elements
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: starting: "
					       "json_array: JSON_OPEN_BRACKET json_elements JSON_CLOSE_BRACKET");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: $json_elements type: %s",
					       json_item_type_name($json_elements));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: about to perform: "
					       "$json_array = parse_json_array($json_elements);");
	}

	/* action */
	$json_array = parse_json_array($json_elements);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_array: returning $json_array type: %s",
					      json_item_type_name($json_array));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_elements, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: ending: "
					       "json_array: JSON_OPEN_BRACKET json_elements JSON_CLOSE_BRACKET");
	}
    }
    |

    JSON_OPEN_BRACKET JSON_CLOSE_BRACKET
    {
	/*
	 * $$ = $json_array
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: starting: "
					       "json_array: JSON_OPEN_BRACKET JSON_CLOSE_BRACKET");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: about to perform: "
					     "$json_array = json_create_array();");
	}

	/* action */
	$json_array = json_create_array();

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_array: returning $json_array type: %s",
					      json_item_type_name($json_array));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_array, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_array: ending: "
					       "json_array: JSON_OPEN_BRACKET JSON_CLOSE_BRACKET");
	}
    }
    ;

json_elements:

    json_element
    {
	/*
	 * $$ = $json_elements
	 * $1 = $json_element
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: starting: "
					       "json_elements: json_element");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: $json_element type: %s",
					       json_item_type_name($json_element));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: about to perform: "
					       "$json_elements = json_create_elements();");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: about to also perform: "
					       "$json_elements = json_elements_add_value($json_elements, $json_element);");
	}

	/* action */
	$json_elements = json_create_elements();
	$json_elements = json_elements_add_value($json_elements, $json_element);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_elements: returning $json_elements type: %s",
					      json_item_type_name($json_elements));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_elements, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: ending: "
					       "json_elements: json_element");
	}
    }
    |

    json_elements JSON_COMMA json_element
    {
	/*
	 * $$ = $json_elements
	 * $3 = $json_element
	 *
	 * NOTE: Cannot use $json_elements due to ambiguity.
	 *	 But we can use $3 for $json_element.
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: starting: "
					       "json_elements: json_elements JSON_COMMA json_element");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: $1 ($json_elements) type: %s",
					       json_item_type_name($1));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: $3 ($json_element) type: %s",
					       json_item_type_name($3));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: about to perform: "
					       "$$ = json_elements_add_value($1, $json_element);");
	}

	/* action */
	$$ = json_elements_add_value($1, $json_element);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_elements: returning $$ ($json_elements) type: %s",
					      json_item_type_name($$));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $$, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_elements: ending: "
					       "json_elements: json_elements JSON_COMMA json_element");
	}
    }
    ;

json_element:

    json_value
    {
	/*
	 * $$ = $json_element
	 * $1 = $json_value
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_element: starting: "
					       "json_element: json_value");
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_element: $json_value type: %s",
					       json_item_type_name($json_value));
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_element: about to perform: "
					       "$json_element = $json_value;");
	}

	/* action */
	$json_element = $json_value; /* magic: json_element becomes json_value's type */

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_element: returning $json_element type: %s",
					      json_item_type_name($json_element));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_element, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_element: ending: "
					       "json_element: json_value");
	}
    }
    ;

json_string:

    JSON_STRING
    {
	/*
	 * $$ = $json_string
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_string: starting: "
					       "json_string: JSON_STRING");
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_string: yytext: <%s>", yytext);
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_string: yyleng: <%d>", yyleng);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_string: about to perform: "
					       "$json_string = parse_json_string(yytext, yyleng);");
	}

	/* action */
	$json_string = parse_json_string(yytext, yyleng);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_string: returning $json_string type: %s",
					      json_item_type_name($json_string));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_string, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_string: ending: "
					       "json_string: JSON_STRING");
	}
    }
    ;

json_number:

    JSON_NUMBER
    {
	/*
	 * $$ = $json_number
	 */

	/* pre-action debugging */
	if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_number: starting: "
					       "json_number: JSON_NUMBER");
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_number: yytext: <%s>", yytext);
	    json_dbg(JSON_DBG_VVHIGH, __func__, "under json_number: yyleng: <%d>", yyleng);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_number: about to perform: "
					       "$json_number = parse_json_number(yytext);");
	}

	/* action */
	$json_number = parse_json_number(yytext);

	/* post-action debugging */
	if (json_dbg_allowed(JSON_DBG_HIGH)) {
	    json_dbg(JSON_DBG_HIGH, __func__, "under json_number: returning $json_number type: %s",
					      json_item_type_name($json_number));
	    json_dbg_tree_print(JSON_DBG_HIGH, __func__, $json_number, JSON_DEFAULT_MAX_DEPTH);
	    json_dbg(JSON_DBG_VHIGH, __func__, "under json_number: ending: "
					       "json_number: JSON_NUMBER");
	}
    }
    ;


%%


/* Section 3: C code */


/*
 * yyerror	- generate an error message for the scanner/parser
 *
 * given:
 *
 *	yyltype	    location type
 *	node	    pointer to struct json * or NULL
 *	format	    printf style format string
 *	...	    optional parameters based on the format
 *
 */
void
yyerror(YYLTYPE *yyltype, struct json **node, char const *format, ...)
{
    va_list ap;		/* variable argument list */
    int ret;		/* libc function return value */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, format);

    UNUSED_ARG(yyltype);

    /*
     * generate an error message for the JSON parser and scanner
     */
    vfpr(stderr, __func__, format, ap);
    if (node != NULL && *node != NULL) {
	fprint(stderr, " type: %s ", json_item_type_name(*node));
    }
    if (yytext != NULL && *yytext != '\0') {
	fprint(stderr, " line: %d: %s\n", yylineno, yytext);
    } else if (yytext == NULL) {
	fprint(stderr, " line: %d: text == NULL\n", yylineno);
    } else {
	fprint(stderr, " line: %d: empty text\n", yylineno);
    }

    /*
     * flush stderr
     */
    clearerr(stderr);           /* pre-clear ferror() status */
    errno = 0;                  /* pre-clear errno for warnp() */
    ret = fflush(stderr);
    if (ret == EOF) {
        if (ferror(stderr)) {
            warnp(__func__, "called from %s: error flushing stream", __func__);
        } else if (feof(stderr)) {
            warnp(__func__, "called from %s: EOF while flushing stream", __func__);
        } else {
            warnp(__func__, "called from %s: unexpected fflush error while flushing stream", __func__);
        }
    }

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
}
