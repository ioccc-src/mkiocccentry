/* json_parser - bison grammar
 *
 * XXX This is VERY incomplete and there are errors; it's VERY MUCH a work in
 * progress (oxymoron though that is) and the issues will be resolved in
 * time.
 *
 * Even the grammar is incomplete and there are no actions yet. I'm sure some of
 * what exists is also wrong.
 *
 * I'm not sure when I will be adding the actions and it's very possible that I
 * won't add all at once.
 *
 * Right now it's very easy to trigger a parser error because it's very
 * incomplete. To call this a first draft is to compliment it; this is simply
 * a starting point for much more to come at a later time.
 */

/* Section 1: Declarations */
%{
#include <inttypes.h>
#include <stdio.h>

#include "json_parser.h"

int yylex(void);
void yyerror(char const *error);
extern int yylineno;
extern char *yytext;
#define YYDEBUG 1
int yydebug = 1;
%}

/*
 * Token types.
 *
 * It's very possible that this is incomplete and everything here is subject to
 * change.
 */
%token JSON_WHITESPACE JSON_LETTER
%token JSON_SIGN JSON_EQUALS JSON_DIGIT JSON_DIGITS JSON_INTEGER JSON_EXPONENT
%token JSON_OPEN_BRACE JSON_CLOSE_BRACE JSON_OPEN_BRACKET JSON_CLOSE_BRACKET
%token JSON_COMMA JSON_COLON JSON_TRUE JSON_FALSE JSON_NULL
%token JSON_STRING JSON_NUMBER

/* This union is NOT correct and it absolutely will change! */
%union json_type {
  char *string;
  uintmax_t uintmax;
  intmax_t number;
  bool boolean;
}

/* Section 2: Rules
 *
 * Not all rules are here and no actions are defined yet. As well some of the
 * rules are probably wrong.
 *
 * Again this is very incomplete and there are errors!
 */
%%
json:		%empty |
		json_element ;

json_value:	json_object |
		json_array  |
		JSON_STRING |
		JSON_NUMBER |
		JSON_TRUE   |
		JSON_FALSE  |
		JSON_NULL
		;

json_object:	JSON_OPEN_BRACE JSON_WHITESPACE JSON_CLOSE_BRACE |
		JSON_OPEN_BRACE json_members JSON_CLOSE_BRACE
		;

json_members:	json_member |
		json_member JSON_COMMA json_members
		;

json_member:	JSON_WHITESPACE JSON_STRING JSON_WHITESPACE JSON_COLON json_element
		;

json_array:	JSON_OPEN_BRACKET JSON_WHITESPACE JSON_CLOSE_BRACKET |
		JSON_OPEN_BRACKET json_elements JSON_CLOSE_BRACKET
		;

json_elements:	json_element |
		json_element JSON_COMMA json_elements
		;

json_element:	JSON_WHITESPACE json_value JSON_WHITESPACE
		;



%%

/* Section 3: C code */
int
main(void)
{
    yyparse();
}

void
yyerror(char const *err)
{
    /*
     * We use dbg() instead of err() but in the future it'll probably use the
     * jerr() function or some other error function. Right now it doesn't use
     * err() because I don't believe the seqcexit tool acts on .l/.y files so
     * the codes wouldn't be sequenced.
     *
     * Currently all you have to do to trigger this function being called is
     * typing anything after the program starts as once more it's incomplete!
     */
    dbg(DBG_NONE, "JSON parser error: %s\n", err);
}
