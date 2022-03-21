/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_JSON_PARSER_TAB_H_INCLUDED
# define YY_YY_JSON_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    JSON_WHITESPACE = 258,         /* JSON_WHITESPACE  */
    JSON_LETTER = 259,             /* JSON_LETTER  */
    JSON_SIGN = 260,               /* JSON_SIGN  */
    JSON_EQUALS = 261,             /* JSON_EQUALS  */
    JSON_DIGIT = 262,              /* JSON_DIGIT  */
    JSON_DIGITS = 263,             /* JSON_DIGITS  */
    JSON_INTEGER = 264,            /* JSON_INTEGER  */
    JSON_EXPONENT = 265,           /* JSON_EXPONENT  */
    JSON_OPEN_BRACE = 266,         /* JSON_OPEN_BRACE  */
    JSON_CLOSE_BRACE = 267,        /* JSON_CLOSE_BRACE  */
    JSON_OPEN_BRACKET = 268,       /* JSON_OPEN_BRACKET  */
    JSON_CLOSE_BRACKET = 269,      /* JSON_CLOSE_BRACKET  */
    JSON_COMMA = 270,              /* JSON_COMMA  */
    JSON_COLON = 271,              /* JSON_COLON  */
    JSON_TRUE = 272,               /* JSON_TRUE  */
    JSON_FALSE = 273,              /* JSON_FALSE  */
    JSON_NULL = 274,               /* JSON_NULL  */
    JSON_STRING = 275,             /* JSON_STRING  */
    JSON_NUMBER = 276              /* JSON_NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
#line 25 "json_parser.y"
union json_type
{
#line 25 "json_parser.y"

  char *text;
  uintmax_t uintmax;
  intmax_t intmax;
  bool boolean;

#line 93 "json_parser.tab.h"

};
#line 25 "json_parser.y"
typedef union json_type YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_JSON_PARSER_TAB_H_INCLUDED  */
