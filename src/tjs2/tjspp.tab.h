namespace TJS {
/* A Bison parser, made by GNU Bison 1.875b.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PT_LPARENTHESIS = 258,
     PT_RPARENTHESIS = 259,
     PT_ERROR = 260,
     PT_COMMA = 261,
     PT_EQUAL = 262,
     PT_NOTEQUAL = 263,
     PT_EQUALEQUAL = 264,
     PT_LOGICALOR = 265,
     PT_LOGICALAND = 266,
     PT_VERTLINE = 267,
     PT_CHEVRON = 268,
     PT_AMPERSAND = 269,
     PT_LT = 270,
     PT_GT = 271,
     PT_LTOREQUAL = 272,
     PT_GTOREQUAL = 273,
     PT_PLUS = 274,
     PT_MINUS = 275,
     PT_ASTERISK = 276,
     PT_SLASH = 277,
     PT_PERCENT = 278,
     PT_EXCLAMATION = 279,
     PT_UN = 280,
     PT_SYMBOL = 281,
     PT_NUM = 282
   };
#endif
#define PT_LPARENTHESIS 258
#define PT_RPARENTHESIS 259
#define PT_ERROR 260
#define PT_COMMA 261
#define PT_EQUAL 262
#define PT_NOTEQUAL 263
#define PT_EQUALEQUAL 264
#define PT_LOGICALOR 265
#define PT_LOGICALAND 266
#define PT_VERTLINE 267
#define PT_CHEVRON 268
#define PT_AMPERSAND 269
#define PT_LT 270
#define PT_GT 271
#define PT_LTOREQUAL 272
#define PT_GTOREQUAL 273
#define PT_PLUS 274
#define PT_MINUS 275
#define PT_ASTERISK 276
#define PT_SLASH 277
#define PT_PERCENT 278
#define PT_EXCLAMATION 279
#define PT_UN 280
#define PT_SYMBOL 281
#define PT_NUM 282




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 42 "tjspp.y"
typedef union YYSTYPE {
	tjs_int32		val;
	tjs_int			nv;
} YYSTYPE;
/* Line 1252 of yacc.c.  */
#line 96 "tjspp.tab.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif






}
