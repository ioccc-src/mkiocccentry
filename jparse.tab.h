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

#ifndef YY_YY_JPARSE_TAB_H_INCLUDED
# define YY_YY_JPARSE_TAB_H_INCLUDED
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
    JSON_LETTER = 258,             /* JSON_LETTER  */
    JSON_SIGN = 259,               /* JSON_SIGN  */
    JSON_DIGIT = 260,              /* JSON_DIGIT  */
    JSON_DIGITS = 261,             /* JSON_DIGITS  */
    JSON_EXPONENT = 262,           /* JSON_EXPONENT  */
    JSON_OPEN_BRACE = 263,         /* JSON_OPEN_BRACE  */
    JSON_CLOSE_BRACE = 264,        /* JSON_CLOSE_BRACE  */
    JSON_OPEN_BRACKET = 265,       /* JSON_OPEN_BRACKET  */
    JSON_CLOSE_BRACKET = 266,      /* JSON_CLOSE_BRACKET  */
    JSON_COMMA = 267,              /* JSON_COMMA  */
    JSON_COLON = 268,              /* JSON_COLON  */
    JSON_NULL = 269,               /* JSON_NULL  */
    JSON_STRING = 270,             /* JSON_STRING  */
    JSON_INTMAX = 271,             /* JSON_INTMAX  */
    JSON_UINTMAX = 272,            /* JSON_UINTMAX  */
    JSON_LONG_DOUBLE = 273,        /* JSON_LONG_DOUBLE  */
    JSON_BOOLEAN = 274             /* JSON_BOOLEAN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
#line 87 "jparse.y"
union json_type
{
#line 87 "jparse.y"

  char *string;
  uintmax_t uintmax;
  intmax_t intmax;
  long double ldouble;
  bool boolean;

#line 92 "jparse.tab.h"

};
#line 87 "jparse.y"
typedef union json_type YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_JPARSE_TAB_H_INCLUDED  */
