#include "tjsCommHead.h"
#pragma hdrstop
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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_COMMA = 258,
     T_EQUAL = 259,
     T_AMPERSANDEQUAL = 260,
     T_VERTLINEEQUAL = 261,
     T_CHEVRONEQUAL = 262,
     T_MINUSEQUAL = 263,
     T_PLUSEQUAL = 264,
     T_PERCENTEQUAL = 265,
     T_SLASHEQUAL = 266,
     T_BACKSLASHEQUAL = 267,
     T_ASTERISKEQUAL = 268,
     T_LOGICALOREQUAL = 269,
     T_LOGICALANDEQUAL = 270,
     T_RARITHSHIFTEQUAL = 271,
     T_LARITHSHIFTEQUAL = 272,
     T_RBITSHIFTEQUAL = 273,
     T_QUESTION = 274,
     T_LOGICALOR = 275,
     T_LOGICALAND = 276,
     T_VERTLINE = 277,
     T_CHEVRON = 278,
     T_AMPERSAND = 279,
     T_NOTEQUAL = 280,
     T_EQUALEQUAL = 281,
     T_DISCNOTEQUAL = 282,
     T_DISCEQUAL = 283,
     T_SWAP = 284,
     T_LT = 285,
     T_GT = 286,
     T_LTOREQUAL = 287,
     T_GTOREQUAL = 288,
     T_RARITHSHIFT = 289,
     T_LARITHSHIFT = 290,
     T_RBITSHIFT = 291,
     T_PERCENT = 292,
     T_SLASH = 293,
     T_BACKSLASH = 294,
     T_ASTERISK = 295,
     T_EXCRAMATION = 296,
     T_TILDE = 297,
     T_DECREMENT = 298,
     T_INCREMENT = 299,
     T_NEW = 300,
     T_DELETE = 301,
     T_TYPEOF = 302,
     T_PLUS = 303,
     T_MINUS = 304,
     T_SHARP = 305,
     T_DOLLAR = 306,
     T_ISVALID = 307,
     T_INVALIDATE = 308,
     T_INSTANCEOF = 309,
     T_LPARENTHESIS = 310,
     T_DOT = 311,
     T_LBRACKET = 312,
     T_THIS = 313,
     T_SUPER = 314,
     T_GLOBAL = 315,
     T_RBRACKET = 316,
     T_CLASS = 317,
     T_RPARENTHESIS = 318,
     T_COLON = 319,
     T_SEMICOLON = 320,
     T_LBRACE = 321,
     T_RBRACE = 322,
     T_CONTINUE = 323,
     T_FUNCTION = 324,
     T_DEBUGGER = 325,
     T_DEFAULT = 326,
     T_CASE = 327,
     T_EXTENDS = 328,
     T_FINALLY = 329,
     T_PROPERTY = 330,
     T_PRIVATE = 331,
     T_PUBLIC = 332,
     T_PROTECTED = 333,
     T_STATIC = 334,
     T_RETURN = 335,
     T_BREAK = 336,
     T_EXPORT = 337,
     T_IMPORT = 338,
     T_SWITCH = 339,
     T_IN = 340,
     T_INCONTEXTOF = 341,
     T_FOR = 342,
     T_WHILE = 343,
     T_DO = 344,
     T_IF = 345,
     T_VAR = 346,
     T_CONST = 347,
     T_ENUM = 348,
     T_GOTO = 349,
     T_THROW = 350,
     T_TRY = 351,
     T_SETTER = 352,
     T_GETTER = 353,
     T_ELSE = 354,
     T_CATCH = 355,
     T_OMIT = 356,
     T_SYNCHRONIZED = 357,
     T_WITH = 358,
     T_INT = 359,
     T_REAL = 360,
     T_STRING = 361,
     T_OCTET = 362,
     T_FALSE = 363,
     T_NULL = 364,
     T_TRUE = 365,
     T_VOID = 366,
     T_NAN = 367,
     T_INFINITY = 368,
     T_UPLUS = 369,
     T_UMINUS = 370,
     T_EVAL = 371,
     T_SYMBOL = 372,
     T_POSTDECREMENT = 373,
     T_POSTINCREMENT = 374,
     T_SUBSTANCE = 375,
     T_ARG = 376,
     T_EXPANDARG = 377,
     T_INLINEARRAY = 378,
     T_ARRAYARG = 379,
     T_INLINEDIC = 380,
     T_DICELM = 381,
     T_REGEXP = 382,
     T_WITHDOT = 383,
     T_THIS_PROXY = 384,
     T_WITHDOT_PROXY = 385,
     T_CONSTVAL = 386
   };
#endif
#define T_COMMA 258
#define T_EQUAL 259
#define T_AMPERSANDEQUAL 260
#define T_VERTLINEEQUAL 261
#define T_CHEVRONEQUAL 262
#define T_MINUSEQUAL 263
#define T_PLUSEQUAL 264
#define T_PERCENTEQUAL 265
#define T_SLASHEQUAL 266
#define T_BACKSLASHEQUAL 267
#define T_ASTERISKEQUAL 268
#define T_LOGICALOREQUAL 269
#define T_LOGICALANDEQUAL 270
#define T_RARITHSHIFTEQUAL 271
#define T_LARITHSHIFTEQUAL 272
#define T_RBITSHIFTEQUAL 273
#define T_QUESTION 274
#define T_LOGICALOR 275
#define T_LOGICALAND 276
#define T_VERTLINE 277
#define T_CHEVRON 278
#define T_AMPERSAND 279
#define T_NOTEQUAL 280
#define T_EQUALEQUAL 281
#define T_DISCNOTEQUAL 282
#define T_DISCEQUAL 283
#define T_SWAP 284
#define T_LT 285
#define T_GT 286
#define T_LTOREQUAL 287
#define T_GTOREQUAL 288
#define T_RARITHSHIFT 289
#define T_LARITHSHIFT 290
#define T_RBITSHIFT 291
#define T_PERCENT 292
#define T_SLASH 293
#define T_BACKSLASH 294
#define T_ASTERISK 295
#define T_EXCRAMATION 296
#define T_TILDE 297
#define T_DECREMENT 298
#define T_INCREMENT 299
#define T_NEW 300
#define T_DELETE 301
#define T_TYPEOF 302
#define T_PLUS 303
#define T_MINUS 304
#define T_SHARP 305
#define T_DOLLAR 306
#define T_ISVALID 307
#define T_INVALIDATE 308
#define T_INSTANCEOF 309
#define T_LPARENTHESIS 310
#define T_DOT 311
#define T_LBRACKET 312
#define T_THIS 313
#define T_SUPER 314
#define T_GLOBAL 315
#define T_RBRACKET 316
#define T_CLASS 317
#define T_RPARENTHESIS 318
#define T_COLON 319
#define T_SEMICOLON 320
#define T_LBRACE 321
#define T_RBRACE 322
#define T_CONTINUE 323
#define T_FUNCTION 324
#define T_DEBUGGER 325
#define T_DEFAULT 326
#define T_CASE 327
#define T_EXTENDS 328
#define T_FINALLY 329
#define T_PROPERTY 330
#define T_PRIVATE 331
#define T_PUBLIC 332
#define T_PROTECTED 333
#define T_STATIC 334
#define T_RETURN 335
#define T_BREAK 336
#define T_EXPORT 337
#define T_IMPORT 338
#define T_SWITCH 339
#define T_IN 340
#define T_INCONTEXTOF 341
#define T_FOR 342
#define T_WHILE 343
#define T_DO 344
#define T_IF 345
#define T_VAR 346
#define T_CONST 347
#define T_ENUM 348
#define T_GOTO 349
#define T_THROW 350
#define T_TRY 351
#define T_SETTER 352
#define T_GETTER 353
#define T_ELSE 354
#define T_CATCH 355
#define T_OMIT 356
#define T_SYNCHRONIZED 357
#define T_WITH 358
#define T_INT 359
#define T_REAL 360
#define T_STRING 361
#define T_OCTET 362
#define T_FALSE 363
#define T_NULL 364
#define T_TRUE 365
#define T_VOID 366
#define T_NAN 367
#define T_INFINITY 368
#define T_UPLUS 369
#define T_UMINUS 370
#define T_EVAL 371
#define T_SYMBOL 372
#define T_POSTDECREMENT 373
#define T_POSTINCREMENT 374
#define T_SUBSTANCE 375
#define T_ARG 376
#define T_EXPANDARG 377
#define T_INLINEARRAY 378
#define T_ARRAYARG 379
#define T_INLINEDIC 380
#define T_DICELM 381
#define T_REGEXP 382
#define T_WITHDOT 383
#define T_THIS_PROXY 384
#define T_WITHDOT_PROXY 385
#define T_CONSTVAL 386




/* Copy the first part of user declarations.  */
#line 1 "syntax/tjs.y"

/*---------------------------------------------------------------------------*/
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
/*---------------------------------------------------------------------------*/
/* tjs.y */
/* TJS2 bison input file */


#include <malloc.h>


#include "tjsInterCodeGen.h"
#include "tjsScriptBlock.h"
#include "tjsError.h"

#define YYERROR_VERBOSE 1

/* param */
#define YYPARSE_PARAM pm
#define YYLEX_PARAM pm

/* script block */
#define sb ((tTJSScriptBlock*)pm)

/* current context */
#define cc (sb->GetCurrentContext())

/* current node */
#define cn (cc->GetCurrentNode())

/* lexical analyzer */
#define lx (sb->GetLexicalAnalyzer())


namespace TJS {

/* yylex/yyerror prototype decl */
#define YYLEX_PROTO_DECL int yylex(YYSTYPE *yylex, void *pm);

int __yyerror(char * msg, void *pm);


#define yyerror(msg) __yyerror(msg, pm);



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 56 "syntax/tjs.y"
typedef union YYSTYPE {
	tjs_int			num;
	tTJSExprNode *		np;
} YYSTYPE;
YYLEX_PROTO_DECL

/* Line 191 of yacc.c.  */
#line 394 "tjs.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 406 "tjs.tab.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1362

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  132
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  97
/* YYNRULES -- Number of rules. */
#define YYNRULES  236
/* YYNRULES -- Number of states. */
#define YYNSTATES  401

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   386

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    13,    17,    19,
      21,    23,    26,    28,    30,    32,    34,    36,    39,    42,
      45,    47,    49,    51,    53,    55,    57,    59,    61,    63,
      65,    66,    71,    72,    73,    81,    82,    83,    93,    94,
      95,   103,   104,   109,   119,   120,   121,   124,   126,   127,
     129,   130,   132,   135,   138,   141,   143,   147,   149,   153,
     154,   160,   161,   166,   167,   171,   175,   181,   182,   184,
     186,   190,   192,   196,   198,   201,   202,   209,   211,   214,
     215,   222,   223,   227,   231,   233,   234,   240,   241,   244,
     245,   251,   253,   257,   259,   262,   266,   267,   274,   275,
     282,   286,   289,   290,   296,   298,   302,   307,   311,   313,
     315,   319,   321,   325,   327,   331,   335,   339,   343,   347,
     351,   355,   359,   363,   367,   371,   375,   379,   383,   387,
     391,   393,   399,   401,   405,   407,   411,   413,   417,   419,
     423,   425,   429,   431,   435,   439,   443,   447,   449,   453,
     457,   461,   465,   467,   471,   475,   479,   481,   485,   489,
     491,   495,   499,   503,   506,   509,   511,   514,   517,   520,
     523,   526,   529,   532,   535,   538,   541,   544,   547,   550,
     553,   556,   560,   565,   568,   573,   576,   581,   584,   586,
     590,   592,   596,   601,   603,   604,   609,   612,   615,   618,
     619,   623,   625,   627,   629,   631,   633,   635,   637,   639,
     641,   642,   646,   647,   651,   656,   658,   660,   664,   665,
     667,   669,   671,   672,   677,   679,   683,   684,   686,   687,
     694,   695,   697,   701,   705,   709,   710
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     133,     0,    -1,   134,    -1,    -1,   135,   136,    -1,    -1,
     136,   137,    -1,   136,     1,    65,    -1,   138,    -1,   139,
      -1,    65,    -1,   194,    65,    -1,   147,    -1,   150,    -1,
     141,    -1,   144,    -1,   152,    -1,    81,    65,    -1,    68,
      65,    -1,    70,    65,    -1,   157,    -1,   161,    -1,   170,
      -1,   177,    -1,   183,    -1,   184,    -1,   186,    -1,   188,
      -1,   189,    -1,   192,    -1,    -1,    66,   140,   136,    67,
      -1,    -1,    -1,    88,   142,    55,   194,    63,   143,   137,
      -1,    -1,    -1,    89,   145,   137,    88,    55,   194,    63,
     146,    65,    -1,    -1,    -1,    90,    55,   148,   194,   149,
      63,   137,    -1,    -1,   147,    99,   151,   137,    -1,    87,
      55,   153,    65,   155,    65,   156,    63,   137,    -1,    -1,
      -1,   154,   158,    -1,   194,    -1,    -1,   194,    -1,    -1,
     194,    -1,   158,    65,    -1,    91,   159,    -1,    92,   159,
      -1,   160,    -1,   159,     3,   160,    -1,   117,    -1,   117,
       4,   193,    -1,    -1,    69,   117,   162,   165,   139,    -1,
      -1,    69,   164,   165,   139,    -1,    -1,    55,   169,    63,
      -1,    55,   166,    63,    -1,    55,   167,     3,   169,    63,
      -1,    -1,   167,    -1,   168,    -1,   167,     3,   168,    -1,
     117,    -1,   117,     4,   193,    -1,    40,    -1,   117,    40,
      -1,    -1,    75,   117,    66,   171,   172,    67,    -1,   173,
      -1,   172,   173,    -1,    -1,    97,    55,   117,    63,   174,
     139,    -1,    -1,   176,   175,   139,    -1,    98,    55,    63,
      -1,    98,    -1,    -1,    62,   117,   178,   179,   139,    -1,
      -1,    73,   193,    -1,    -1,    73,   193,     3,   180,   181,
      -1,   182,    -1,   181,     3,   182,    -1,   193,    -1,    80,
      65,    -1,    80,   194,    65,    -1,    -1,    84,    55,   194,
      63,   185,   139,    -1,    -1,   103,    55,   194,    63,   187,
     137,    -1,    72,   194,    64,    -1,    71,    64,    -1,    -1,
      96,   190,   137,   191,   137,    -1,   100,    -1,   100,    55,
      63,    -1,   100,    55,   117,    63,    -1,    95,   194,    65,
      -1,   196,    -1,   195,    -1,   195,    90,   194,    -1,   196,
      -1,   195,     3,   196,    -1,   197,    -1,   197,    29,   196,
      -1,   197,     4,   196,    -1,   197,     5,   196,    -1,   197,
       6,   196,    -1,   197,     7,   196,    -1,   197,     8,   196,
      -1,   197,     9,   196,    -1,   197,    10,   196,    -1,   197,
      11,   196,    -1,   197,    12,   196,    -1,   197,    13,   196,
      -1,   197,    14,   196,    -1,   197,    15,   196,    -1,   197,
      18,   196,    -1,   197,    17,   196,    -1,   197,    16,   196,
      -1,   198,    -1,   198,    19,   197,    64,   197,    -1,   199,
      -1,   198,    20,   199,    -1,   200,    -1,   199,    21,   200,
      -1,   201,    -1,   200,    22,   201,    -1,   202,    -1,   201,
      23,   202,    -1,   203,    -1,   202,    24,   203,    -1,   204,
      -1,   203,    25,   204,    -1,   203,    26,   204,    -1,   203,
      27,   204,    -1,   203,    28,   204,    -1,   205,    -1,   204,
      30,   205,    -1,   204,    31,   205,    -1,   204,    32,   205,
      -1,   204,    33,   205,    -1,   206,    -1,   205,    34,   206,
      -1,   205,    35,   206,    -1,   205,    36,   206,    -1,   207,
      -1,   206,    48,   207,    -1,   206,    49,   207,    -1,   209,
      -1,   207,    37,   209,    -1,   207,    38,   209,    -1,   207,
      39,   209,    -1,   208,   209,    -1,   207,    40,    -1,   210,
      -1,    41,   209,    -1,    42,   209,    -1,    43,   209,    -1,
      44,   209,    -1,    45,   217,    -1,    53,   209,    -1,    52,
     209,    -1,   210,    52,    -1,    46,   209,    -1,    47,   209,
      -1,    50,   209,    -1,    51,   209,    -1,    48,   209,    -1,
      49,   209,    -1,    40,   209,    -1,   210,    54,   209,    -1,
      55,   104,    63,   209,    -1,   104,   209,    -1,    55,   105,
      63,   209,    -1,   105,   209,    -1,    55,   106,    63,   209,
      -1,   106,   209,    -1,   211,    -1,   211,    86,   210,    -1,
     214,    -1,    55,   194,    63,    -1,   211,    57,   194,    61,
      -1,   217,    -1,    -1,   211,    56,   212,   117,    -1,   211,
      44,    -1,   211,    43,    -1,   211,    41,    -1,    -1,    56,
     213,   117,    -1,   131,    -1,   117,    -1,    58,    -1,    59,
      -1,   163,    -1,    60,    -1,   111,    -1,   220,    -1,   224,
      -1,    -1,    11,   215,   127,    -1,    -1,    38,   216,   127,
      -1,   211,    55,   218,    63,    -1,   101,    -1,   219,    -1,
     218,     3,   219,    -1,    -1,    40,    -1,   208,    -1,   193,
      -1,    -1,    57,   221,   222,    61,    -1,   223,    -1,   222,
       3,   223,    -1,    -1,   193,    -1,    -1,    37,    57,   225,
     226,   228,    61,    -1,    -1,   227,    -1,   226,     3,   227,
      -1,   193,     3,   193,    -1,   117,    64,   193,    -1,    -1,
       3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   212,   212,   217,   217,   223,   225,   226,   233,   234,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     263,   263,   270,   271,   270,   277,   280,   277,   286,   287,
     286,   293,   293,   299,   309,   310,   310,   312,   318,   319,
     324,   325,   330,   334,   335,   342,   343,   348,   350,   356,
     356,   366,   366,   379,   381,   382,   383,   387,   389,   393,
     394,   398,   400,   405,   407,   419,   418,   427,   428,   432,
     432,   440,   440,   449,   450,   456,   456,   463,   465,   466,
     466,   471,   472,   476,   481,   482,   489,   488,   496,   495,
     502,   503,   508,   508,   515,   516,   517,   523,   528,   532,
     533,   538,   539,   544,   545,   546,   547,   548,   549,   550,
     551,   552,   553,   554,   555,   556,   557,   558,   559,   560,
     565,   566,   574,   575,   579,   580,   585,   586,   590,   591,
     595,   596,   600,   601,   602,   603,   604,   608,   609,   610,
     611,   612,   616,   617,   618,   619,   624,   625,   626,   630,
     631,   632,   633,   634,   638,   642,   643,   644,   645,   646,
     647,   648,   649,   650,   651,   652,   653,   654,   655,   656,
     657,   658,   659,   660,   661,   662,   663,   664,   668,   669,
     674,   675,   676,   677,   678,   678,   682,   683,   684,   685,
     685,   693,   695,   698,   699,   700,   701,   702,   703,   704,
     705,   705,   708,   708,   716,   721,   722,   723,   727,   728,
     729,   730,   736,   736,   745,   746,   751,   752,   757,   757,
     767,   769,   770,   775,   776,   783,   785
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\",\"", "\"=\"", "\"&=\"", "\"|=\"", 
  "\"^=\"", "\"-=\"", "\"+=\"", "\"%=\"", "\"/=\"", "\"\\\\=\"", "\"*=\"", 
  "\"||=\"", "\"&&=\"", "\">>>=\"", "\"<<=\"", "\">>=\"", "\"?\"", 
  "\"||\"", "\"&&\"", "\"|\"", "\"^\"", "\"&\"", "\"!=\"", "\"==\"", 
  "\"!==\"", "\"===\"", "\"<->\"", "\"<\"", "\">\"", "\"<=\"", "\">=\"", 
  "\">>\"", "\"<<\"", "\">>>\"", "\"%\"", "\"/\"", "\"\\\\\"", "\"*\"", 
  "\"!\"", "\"~\"", "\"--\"", "\"++\"", "\"new\"", "\"delete\"", 
  "\"typeof\"", "\"+\"", "\"-\"", "\"#\"", "\"$\"", "\"isvalid\"", 
  "\"invalidate\"", "\"instanceof\"", "\"(\"", "\".\"", "\"[\"", 
  "\"this\"", "\"super\"", "\"global\"", "\"]\"", "\"class\"", "\")\"", 
  "\":\"", "\";\"", "\"{\"", "\"}\"", "\"continue\"", "\"function\"", 
  "\"debugger\"", "\"default\"", "\"case\"", "\"extends\"", "\"finally\"", 
  "\"property\"", "\"private\"", "\"public\"", "\"protected\"", 
  "\"static\"", "\"return\"", "\"break\"", "\"export\"", "\"import\"", 
  "\"switch\"", "\"in\"", "\"incontextof\"", "\"for\"", "\"while\"", 
  "\"do\"", "\"if\"", "\"var\"", "\"const\"", "\"enum\"", "\"goto\"", 
  "\"throw\"", "\"try\"", "\"setter\"", "\"getter\"", "\"else\"", 
  "\"catch\"", "\"...\"", "\"synchronized\"", "\"with\"", "\"int\"", 
  "\"real\"", "\"string\"", "\"octet\"", "\"false\"", "\"null\"", 
  "\"true\"", "\"void\"", "\"NaN\"", "\"Infinity\"", "T_UPLUS", 
  "T_UMINUS", "T_EVAL", "T_SYMBOL", "T_POSTDECREMENT", "T_POSTINCREMENT", 
  "T_SUBSTANCE", "T_ARG", "T_EXPANDARG", "T_INLINEARRAY", "T_ARRAYARG", 
  "T_INLINEDIC", "T_DICELM", "T_REGEXP", "T_WITHDOT", "T_THIS_PROXY", 
  "T_WITHDOT_PROXY", "T_CONSTVAL", "$accept", "program", "global_list", 
  "@1", "def_list", "block_or_statement", "statement", "block", "@2", 
  "while", "@3", "@4", "do_while", "@5", "@6", "if", "@7", "@8", 
  "if_else", "@9", "for", "for_first_clause", "@10", "for_second_clause", 
  "for_third_clause", "variable_def", "variable_def_inner", 
  "variable_id_list", "variable_id", "func_def", "@11", "func_expr_def", 
  "@12", "func_decl_arg_opt", "func_decl_arg_list", 
  "func_decl_arg_at_least_one", "func_decl_arg", "func_decl_arg_collapse", 
  "property_def", "@13", "property_handler_def_list", 
  "property_handler_def", "@14", "@15", "property_getter_handler_head", 
  "class_def", "@16", "class_extender", "@17", "extends_list", 
  "extends_name", "return", "switch", "@18", "with", "@19", "case", "try", 
  "@20", "catch", "throw", "expr_no_comma", "expr", "comma_expr", 
  "assign_expr", "cond_expr", "logical_or_expr", "logical_and_expr", 
  "inclusive_or_expr", "exclusive_or_expr", "and_expr", "identical_expr", 
  "compare_expr", "shift_expr", "add_sub_expr", "mul_div_expr", 
  "mul_div_expr_and_asterisk", "unary_expr", "incontextof_expr", 
  "priority_expr", "@21", "@22", "factor_expr", "@23", "@24", 
  "func_call_expr", "call_arg_list", "call_arg", "inline_array", "@25", 
  "array_elm_list", "array_elm", "inline_dic", "@26", "dic_elm_list", 
  "dic_elm", "dic_dummy_elm_opt", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   132,   133,   135,   134,   136,   136,   136,   137,   137,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   138,
     140,   139,   142,   143,   141,   145,   146,   144,   148,   149,
     147,   151,   150,   152,   153,   154,   153,   153,   155,   155,
     156,   156,   157,   158,   158,   159,   159,   160,   160,   162,
     161,   164,   163,   165,   165,   165,   165,   166,   166,   167,
     167,   168,   168,   169,   169,   171,   170,   172,   172,   174,
     173,   175,   173,   176,   176,   178,   177,   179,   179,   180,
     179,   181,   181,   182,   183,   183,   185,   184,   187,   186,
     188,   188,   190,   189,   191,   191,   191,   192,   193,   194,
     194,   195,   195,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     197,   197,   198,   198,   199,   199,   200,   200,   201,   201,
     202,   202,   203,   203,   203,   203,   203,   204,   204,   204,
     204,   204,   205,   205,   205,   205,   206,   206,   206,   207,
     207,   207,   207,   207,   208,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   210,   210,
     211,   211,   211,   211,   212,   211,   211,   211,   211,   213,
     211,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     215,   214,   216,   214,   217,   218,   218,   218,   219,   219,
     219,   219,   221,   220,   222,   222,   223,   223,   225,   224,
     226,   226,   226,   227,   227,   228,   228
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     2,     0,     2,     3,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     0,     0,     7,     0,     0,     9,     0,     0,
       7,     0,     4,     9,     0,     0,     2,     1,     0,     1,
       0,     1,     2,     2,     2,     1,     3,     1,     3,     0,
       5,     0,     4,     0,     3,     3,     5,     0,     1,     1,
       3,     1,     3,     1,     2,     0,     6,     1,     2,     0,
       6,     0,     3,     3,     1,     0,     5,     0,     2,     0,
       5,     1,     3,     1,     2,     3,     0,     6,     0,     6,
       3,     2,     0,     5,     1,     3,     4,     3,     1,     1,
       3,     1,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     5,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     4,     2,     4,     2,     4,     2,     1,     3,
       1,     3,     4,     1,     0,     4,     2,     2,     2,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     0,     3,     4,     1,     1,     3,     0,     1,
       1,     1,     0,     4,     1,     3,     0,     1,     0,     6,
       0,     1,     3,     3,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     2,     5,     1,     0,     0,   210,     0,   212,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   199,   222,   203,   204,   206,
       0,    10,    30,     0,    61,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    35,     0,     0,     0,     0,   102,
       0,     0,     0,     0,   207,   202,   201,     6,     8,     9,
      14,    15,    12,    13,    16,    20,     0,    21,   205,    22,
      23,    24,    25,    26,    27,    28,    29,     0,   109,   111,
     113,   130,   132,   134,   136,   138,   140,   142,   147,   152,
     156,     0,   159,   165,   188,   190,   193,   208,   209,     7,
       0,   228,     0,    61,   180,   166,   167,   168,   169,     0,
       0,   170,   174,   175,   178,   179,   176,   177,   172,   171,
       0,     0,     0,     0,     0,   226,    85,     5,    18,    59,
      63,    19,   101,     0,     0,    94,     0,    17,     0,    45,
       0,     0,    38,    57,    53,    55,    54,     0,     0,     0,
     183,   185,   187,    41,    52,    11,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   164,   163,   173,     0,
     198,   197,   196,   218,   194,     0,     0,   211,   230,   213,
       0,     0,     0,   191,   200,   227,   108,     0,   224,    87,
       0,    63,    67,     0,   100,    75,    95,     0,     0,     0,
      47,     0,     0,     0,     0,     0,   107,     0,     0,     0,
     112,   110,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   129,   128,   127,   114,     0,   133,
     135,   137,   139,   141,   143,   144,   145,   146,   148,   149,
     150,   151,   153,   154,   155,   157,   158,   160,   161,   162,
     181,   219,   215,   221,   220,     0,   216,     0,     0,   189,
     202,     0,   235,   231,   182,   184,   186,   226,   223,     0,
       0,    31,     0,    73,    71,     0,    68,    69,     0,    62,
       0,    96,    48,    46,     0,     0,    39,    58,    56,   104,
       0,    98,    42,     0,   218,   214,   195,   192,     0,     0,
     236,     0,   225,    88,    86,    60,     0,    74,    65,     0,
      64,     0,    84,     0,    77,    81,     0,     0,    49,    33,
       0,     0,     0,   103,     0,   131,   217,   234,   233,   232,
     229,    89,    72,    70,     0,     0,     0,    76,    78,     0,
      97,    50,     0,     0,     0,   105,     0,    99,     0,    66,
       0,    83,    82,     0,    51,    34,    36,    40,   106,    90,
      91,    93,    79,     0,     0,     0,     0,    43,    37,    92,
      80
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,     3,     5,    57,    58,    59,   127,    60,
     140,   372,    61,   141,   394,    62,   233,   351,    63,   239,
      64,   228,   229,   347,   383,    65,    66,   144,   145,    67,
     221,    68,   130,   223,   305,   306,   307,   308,    69,   310,
     343,   344,   396,   369,   345,    70,   219,   300,   378,   389,
     390,    71,    72,   346,    73,   354,    74,    75,   148,   320,
      76,   215,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
     287,   124,    95,   100,   102,    96,   285,   286,    97,   125,
     217,   218,    98,   208,   292,   293,   331
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -223
static const short yypact[] =
{
    -223,    10,  -223,  -223,  -223,   262,   -51,  -223,   -14,  -223,
    1053,  1053,  1053,  1053,  1053,    81,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1053,  1096,  -223,  -223,  -223,  -223,  -223,
     -44,  -223,  -223,    14,   -33,    26,    33,  1053,   -13,   550,
      40,    51,    61,  -223,  -223,    66,     6,     6,  1053,  -223,
      67,  1053,  1053,  1053,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,    27,  -223,  -223,  -223,    63,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,    69,     0,  -223,
     236,    15,   114,   124,   125,   129,    85,   112,   -18,    28,
     133,  1053,  -223,    12,    31,  -223,  -223,  -223,  -223,  -223,
      22,  -223,    32,  -223,  -223,  -223,  -223,  -223,  -223,  1053,
      52,   111,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
     642,   734,   826,    93,    41,  1053,  -223,  -223,  -223,  -223,
     105,  -223,  -223,   100,    99,  -223,   115,  -223,  1053,   869,
     127,   454,  -223,   175,   187,  -223,   187,   126,   454,  1053,
    -223,  -223,  -223,  -223,  -223,  -223,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,
    1053,  1053,  1053,  1053,  1053,  1053,  -223,  -223,  -223,  1053,
    -223,  -223,  -223,   961,  -223,  1053,    81,  -223,  1188,  -223,
    1053,  1053,  1053,  -223,  -223,  -223,  -223,     4,  -223,   120,
     358,   105,   -36,   128,  -223,  -223,  -223,   132,   131,    -9,
    -223,  1053,   109,  1053,  1053,     6,  -223,   101,   136,   454,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,   141,   114,
     124,   125,   129,    85,   112,   112,   112,   112,   -18,   -18,
     -18,   -18,    28,    28,    28,   133,   133,  -223,  -223,  -223,
    -223,  1053,  -223,  -223,  1053,     5,  -223,    83,   145,  -223,
     149,   214,   220,  -223,  -223,  -223,  -223,  1053,  -223,  1053,
     128,  -223,   128,  -223,    29,   161,   222,  -223,   163,  -223,
       1,  -223,  1053,  -223,   164,   174,  -223,  -223,  -223,   176,
     454,  -223,  -223,  1053,  1231,  -223,  -223,  -223,  1053,  1053,
    1188,   169,  -223,   229,  -223,  -223,  1053,  -223,  -223,   -36,
    -223,   178,   179,   -27,  -223,  -223,   128,   170,  -223,  -223,
    1053,   192,   -54,  -223,   454,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,   193,   122,   195,  -223,  -223,   128,
    -223,  1053,   454,   196,   454,  -223,   197,  -223,  1053,  -223,
     198,  -223,  -223,   203,  -223,  -223,  -223,  -223,  -223,   234,
    -223,  -223,  -223,   454,   202,  1053,   128,  -223,  -223,  -223,
    -223
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -223,  -223,  -223,  -223,   142,  -136,  -223,  -222,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -223,  -223,  -223,  -223,  -223,    39,   223,    36,  -223,
    -223,  -223,  -223,    53,  -223,  -223,   -67,   -64,  -223,  -223,
    -223,   -66,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -119,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,  -223,
    -223,  -167,   -24,  -223,  -114,  -172,  -223,   103,   104,   102,
     106,   107,    -5,     2,  -151,   -90,  -197,     9,    76,   268,
    -223,  -223,  -223,  -223,  -223,   270,  -223,   -37,  -223,  -223,
    -223,    -8,  -223,  -223,  -223,   -39,  -223
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -194
static const short yytable[] =
{
     123,   309,   258,   156,   303,   232,   284,   297,   324,   375,
       4,   216,   237,   133,    99,   136,   188,   189,   190,   104,
     105,   106,   107,   108,   147,   112,   113,   114,   115,   116,
     117,   118,   119,   336,   174,   175,   283,   272,   273,   274,
     367,   291,   240,   101,   242,   243,   244,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     150,   151,   152,   376,   198,   298,   199,   317,   325,   337,
     341,   342,   200,   126,   201,   202,   191,   192,   334,   128,
     335,   304,    46,    47,   129,   123,   203,   204,   205,   216,
     157,   131,     7,   200,   216,   201,   202,   132,   341,   342,
     197,   275,   276,   322,   134,   137,   138,   203,   204,   205,
     180,   181,   182,   183,   227,   230,   139,   206,     8,     9,
     216,   142,   149,   143,   370,   238,   153,   284,   154,   150,
     151,   152,   333,   241,   155,   176,   109,    25,    26,    27,
      28,    29,   184,   185,   186,   187,   177,   382,   178,   207,
     103,   355,  -193,   179,  -193,  -193,   213,   283,   214,   209,
     222,   357,   358,   291,   224,   225,  -193,  -193,  -193,   362,
     193,   194,   195,   196,   400,   264,   265,   266,   267,   234,
     226,   288,   231,   216,   353,   216,   268,   269,   270,   271,
     235,   236,    54,   299,    32,   311,   312,   315,    55,   321,
     326,   319,   277,   278,   279,   323,   327,   314,   280,   316,
     216,   391,    56,   328,   216,   216,   216,   329,   377,   294,
     295,   296,   216,   330,   338,   339,   340,   349,   391,   350,
     360,   352,   361,   365,   366,   371,   385,   395,   387,   380,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   374,   379,   397,   381,   386,
     388,   392,    -4,     6,   216,   173,   393,   398,   313,   220,
     146,   318,   363,     7,   302,   364,   399,   368,   259,   261,
     260,   216,   289,   110,   262,   111,   263,   356,   348,   332,
     104,   359,     0,   197,     0,     0,     0,     0,     0,     8,
       9,     0,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,     0,    24,    25,    26,
      27,    28,    29,     0,    30,     0,   373,    31,    32,     0,
      33,    34,    35,    36,    37,     0,     0,    38,     0,     0,
       0,     0,    39,    40,     0,     0,    41,   384,     0,    42,
      43,    44,    45,    46,    47,     0,     0,    48,    49,     6,
       0,     0,     0,     0,     0,    50,    51,    52,    53,     7,
       0,     0,     0,    54,     0,     0,     0,     0,     0,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    56,     0,     8,     9,     0,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,     0,
      30,     0,     0,    31,    32,   301,    33,    34,    35,    36,
      37,     0,     0,    38,     0,     0,     0,     0,    39,    40,
       0,     0,    41,     0,     0,    42,    43,    44,    45,    46,
      47,     0,     0,    48,    49,     0,     0,     0,     0,     0,
       0,    50,    51,    52,    53,     7,     0,     0,     0,    54,
       0,     0,     0,     0,     0,    55,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,     8,     9,     0,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,     0,    24,
      25,    26,    27,    28,    29,     0,    30,     0,     0,    31,
      32,     0,    33,    34,    35,    36,    37,     0,     0,    38,
       0,     0,     0,     0,    39,    40,     0,     0,    41,     0,
       0,    42,    43,    44,    45,    46,    47,     0,     0,    48,
      49,     0,     0,     0,     0,     0,     0,    50,    51,    52,
      53,     7,     0,     0,     0,    54,     0,     0,     0,     0,
       0,    55,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    56,     0,     8,     9,     0,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,     0,    24,    25,    26,    27,    28,
      29,     0,     0,     0,     0,   135,     0,     0,     0,   103,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,    51,    52,    53,     0,     0,     0,
       0,    54,     0,     0,     0,     0,     0,    55,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     8,
       9,    56,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,     0,    24,    25,    26,
      27,    28,    29,     0,     0,   210,     0,     0,     0,     0,
       0,   103,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,    51,    52,    53,     0,
       0,     0,     0,    54,     0,     0,     0,     0,     0,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     8,     9,    56,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,     0,    24,
      25,    26,    27,    28,    29,     0,     0,   211,     0,     0,
       0,     0,     0,   103,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,    51,    52,
      53,     0,     0,     0,     0,    54,     0,     0,     0,     0,
       0,    55,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,    56,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
       7,    24,    25,    26,    27,    28,    29,     0,     0,   212,
       0,     0,     0,     0,     0,   103,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,     9,     0,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,     0,    24,    25,    26,    27,    28,    29,
      51,    52,    53,     0,   -44,     0,     0,    54,   103,     0,
       0,     0,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    56,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,    51,    52,    53,     0,     0,     0,     0,
      54,     0,     0,     0,     0,     0,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     9,
      56,   281,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,     0,    24,    25,    26,    27,
      28,    29,     0,     0,     0,     0,     0,     0,     0,     0,
     103,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   282,     0,     7,    51,    52,    53,     0,     0,
       0,     0,    54,     0,     0,     0,     0,     0,    55,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,     9,    56,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,     7,    24,    25,
      26,    27,    28,    29,     0,     0,     0,     0,     0,     0,
       0,     0,   103,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,     0,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
       0,    24,    25,    26,    27,    28,    29,    51,    52,    53,
       0,     0,     0,     0,    54,   103,     0,     0,     0,     0,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    56,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
     120,   121,   122,     0,     0,     0,     0,    54,     0,     0,
       0,     0,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     8,     9,    56,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     7,    24,    25,    26,    27,    28,    29,     0,
       0,     0,     0,     0,     0,     0,     0,   103,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     9,
       0,   281,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,     0,    24,    25,    26,    27,
      28,    29,    51,    52,    53,     0,     0,     0,     0,    54,
     103,     0,     0,     0,     0,   290,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,    52,    53,     0,     0,
       0,     0,    54,     0,     0,     0,     0,     0,    55,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    56
};

static const short yycheck[] =
{
      24,   223,   174,     3,    40,   141,   203,     3,     3,    63,
       0,   125,   148,    37,    65,    39,    34,    35,    36,    10,
      11,    12,    13,    14,    48,    16,    17,    18,    19,    20,
      21,    22,    23,     4,    19,    20,   203,   188,   189,   190,
      67,   208,   156,    57,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
      51,    52,    53,   117,    52,    61,    54,   234,    63,    40,
      97,    98,    41,   117,    43,    44,    48,    49,   300,    65,
     302,   117,    91,    92,   117,   109,    55,    56,    57,   203,
      90,    65,    11,    41,   208,    43,    44,    64,    97,    98,
      91,   191,   192,   239,   117,    65,    55,    55,    56,    57,
      25,    26,    27,    28,   138,   139,    55,    86,    37,    38,
     234,    55,    55,   117,   346,   149,    99,   324,    65,   120,
     121,   122,   299,   157,    65,    21,    55,    56,    57,    58,
      59,    60,    30,    31,    32,    33,    22,   369,    23,   127,
      69,   323,    41,    24,    43,    44,    63,   324,   117,   127,
      55,   328,   329,   330,    64,    66,    55,    56,    57,   336,
      37,    38,    39,    40,   396,   180,   181,   182,   183,     4,
      65,   205,    55,   297,   320,   299,   184,   185,   186,   187,
       3,    65,   111,    73,    66,    63,    65,    88,   117,    63,
     117,   100,   193,   194,   195,    64,    61,   231,   199,   233,
     324,   378,   131,    64,   328,   329,   330,     3,   354,   210,
     211,   212,   336,     3,    63,     3,    63,    63,   395,    55,
      61,    55,     3,    55,    55,    65,   372,     3,   374,   117,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    63,    63,   393,    63,    63,
      63,    63,     0,     1,   378,    29,    63,    65,   229,   127,
      47,   235,   339,    11,   221,   339,   395,   343,   175,   177,
     176,   395,   206,    15,   178,    15,   179,   324,   312,   297,
     281,   330,    -1,   284,    -1,    -1,    -1,    -1,    -1,    37,
      38,    -1,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    55,    56,    57,
      58,    59,    60,    -1,    62,    -1,   350,    65,    66,    -1,
      68,    69,    70,    71,    72,    -1,    -1,    75,    -1,    -1,
      -1,    -1,    80,    81,    -1,    -1,    84,   371,    -1,    87,
      88,    89,    90,    91,    92,    -1,    -1,    95,    96,     1,
      -1,    -1,    -1,    -1,    -1,   103,   104,   105,   106,    11,
      -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,   117,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   131,    -1,    37,    38,    -1,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    55,    56,    57,    58,    59,    60,    -1,
      62,    -1,    -1,    65,    66,    67,    68,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    -1,    80,    81,
      -1,    -1,    84,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    95,    96,    -1,    -1,    -1,    -1,    -1,
      -1,   103,   104,   105,   106,    11,    -1,    -1,    -1,   111,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   131,
      -1,    37,    38,    -1,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    55,
      56,    57,    58,    59,    60,    -1,    62,    -1,    -1,    65,
      66,    -1,    68,    69,    70,    71,    72,    -1,    -1,    75,
      -1,    -1,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,
      -1,    87,    88,    89,    90,    91,    92,    -1,    -1,    95,
      96,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,    11,    -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   131,    -1,    37,    38,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    55,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    11,   104,   105,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      38,   131,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    11,   104,   105,   106,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,   117,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    38,   131,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    55,
      56,    57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,   104,   105,
     106,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,   131,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      11,    55,    56,    57,    58,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    38,    -1,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    55,    56,    57,    58,    59,    60,
     104,   105,   106,    -1,    65,    -1,    -1,   111,    69,    -1,
      -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   131,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    11,   104,   105,   106,    -1,    -1,    -1,    -1,
     111,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
     131,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    55,    56,    57,    58,
      59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    11,   104,   105,   106,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,   117,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      37,    38,   131,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    11,    55,    56,
      57,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    38,    -1,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    58,    59,    60,   104,   105,   106,
      -1,    -1,    -1,    -1,   111,    69,    -1,    -1,    -1,    -1,
     117,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   131,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
     104,   105,   106,    -1,    -1,    -1,    -1,   111,    -1,    -1,
      -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,   131,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    11,    55,    56,    57,    58,    59,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    38,
      -1,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    55,    56,    57,    58,
      59,    60,   104,   105,   106,    -1,    -1,    -1,    -1,   111,
      69,    -1,    -1,    -1,    -1,   117,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   131,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   104,   105,   106,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,   117,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   131
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   133,   134,   135,     0,   136,     1,    11,    37,    38,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    55,    56,    57,    58,    59,    60,
      62,    65,    66,    68,    69,    70,    71,    72,    75,    80,
      81,    84,    87,    88,    89,    90,    91,    92,    95,    96,
     103,   104,   105,   106,   111,   117,   131,   137,   138,   139,
     141,   144,   147,   150,   152,   157,   158,   161,   163,   170,
     177,   183,   184,   186,   188,   189,   192,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   214,   217,   220,   224,    65,
     215,    57,   216,    69,   209,   209,   209,   209,   209,    55,
     211,   217,   209,   209,   209,   209,   209,   209,   209,   209,
     104,   105,   106,   194,   213,   221,   117,   140,    65,   117,
     164,    65,    64,   194,   117,    65,   194,    65,    55,    55,
     142,   145,    55,   117,   159,   160,   159,   194,   190,    55,
     209,   209,   209,    99,    65,    65,     3,    90,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    29,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    30,    31,    32,    33,    34,    35,
      36,    48,    49,    37,    38,    39,    40,   209,    52,    54,
      41,    43,    44,    55,    56,    57,    86,   127,   225,   127,
      63,    63,    63,    63,   117,   193,   196,   222,   223,   178,
     136,   162,    55,   165,    64,    66,    65,   194,   153,   154,
     194,    55,   137,   148,     4,     3,    65,   137,   194,   151,
     196,   194,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   197,   199,
     200,   201,   202,   203,   204,   204,   204,   204,   205,   205,
     205,   205,   206,   206,   206,   207,   207,   209,   209,   209,
     209,    40,   101,   193,   208,   218,   219,   212,   194,   210,
     117,   193,   226,   227,   209,   209,   209,     3,    61,    73,
     179,    67,   165,    40,   117,   166,   167,   168,   169,   139,
     171,    63,    65,   158,   194,    88,   194,   193,   160,   100,
     191,    63,   137,    64,     3,    63,   117,    61,    64,     3,
       3,   228,   223,   193,   139,   139,     4,    40,    63,     3,
      63,    97,    98,   172,   173,   176,   185,   155,   194,    63,
      55,   149,    55,   137,   187,   197,   219,   193,   193,   227,
      61,     3,   193,   168,   169,    55,    55,    67,   173,   175,
     139,    65,   143,   194,    63,    63,   117,   137,   180,    63,
     117,    63,   139,   156,   194,   137,    63,   137,    63,   181,
     182,   193,    63,    63,   146,     3,   174,   137,    65,   182,
     139
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 217 "syntax/tjs.y"
    { sb->PushContextStack(TJS_W("global"),
												ctTopLevel); ;}
    break;

  case 4:
#line 219 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 7:
#line 226 "syntax/tjs.y"
    { if(sb->CompileErrorCount>20)
												YYABORT;
											  else yyerrok; ;}
    break;

  case 11:
#line 240 "syntax/tjs.y"
    { cc->CreateExprCode(yyvsp[-1].np); ;}
    break;

  case 17:
#line 246 "syntax/tjs.y"
    { cc->DoBreak(); ;}
    break;

  case 18:
#line 247 "syntax/tjs.y"
    { cc->DoContinue(); ;}
    break;

  case 19:
#line 248 "syntax/tjs.y"
    { cc->DoDebugger(); ;}
    break;

  case 30:
#line 263 "syntax/tjs.y"
    { cc->EnterBlock(); ;}
    break;

  case 31:
#line 265 "syntax/tjs.y"
    { cc->ExitBlock(); ;}
    break;

  case 32:
#line 270 "syntax/tjs.y"
    { cc->EnterWhileCode(false); ;}
    break;

  case 33:
#line 271 "syntax/tjs.y"
    { cc->CreateWhileExprCode(yyvsp[-1].np, false); ;}
    break;

  case 34:
#line 272 "syntax/tjs.y"
    { cc->ExitWhileCode(false); ;}
    break;

  case 35:
#line 277 "syntax/tjs.y"
    { cc->EnterWhileCode(true); ;}
    break;

  case 36:
#line 280 "syntax/tjs.y"
    { cc->CreateWhileExprCode(yyvsp[-1].np, true); ;}
    break;

  case 37:
#line 281 "syntax/tjs.y"
    { cc->ExitWhileCode(true); ;}
    break;

  case 38:
#line 286 "syntax/tjs.y"
    { cc->EnterIfCode(); ;}
    break;

  case 39:
#line 287 "syntax/tjs.y"
    { cc->CreateIfExprCode(yyvsp[0].np); ;}
    break;

  case 40:
#line 288 "syntax/tjs.y"
    { cc->ExitIfCode(); ;}
    break;

  case 41:
#line 293 "syntax/tjs.y"
    { cc->EnterElseCode(); ;}
    break;

  case 42:
#line 294 "syntax/tjs.y"
    { cc->ExitElseCode(); ;}
    break;

  case 43:
#line 303 "syntax/tjs.y"
    { cc->ExitForCode(); ;}
    break;

  case 44:
#line 309 "syntax/tjs.y"
    { cc->EnterForCode(false); ;}
    break;

  case 45:
#line 310 "syntax/tjs.y"
    { cc->EnterForCode(true); ;}
    break;

  case 47:
#line 312 "syntax/tjs.y"
    { cc->EnterForCode(false);
											  cc->CreateExprCode(yyvsp[0].np); ;}
    break;

  case 48:
#line 318 "syntax/tjs.y"
    { cc->CreateForExprCode(NULL); ;}
    break;

  case 49:
#line 319 "syntax/tjs.y"
    { cc->CreateForExprCode(yyvsp[0].np); ;}
    break;

  case 50:
#line 324 "syntax/tjs.y"
    { cc->SetForThirdExprCode(NULL); ;}
    break;

  case 51:
#line 325 "syntax/tjs.y"
    { cc->SetForThirdExprCode(yyvsp[0].np); ;}
    break;

  case 57:
#line 348 "syntax/tjs.y"
    { cc->AddLocalVariable(
												lx->GetString(yyvsp[0].num)); ;}
    break;

  case 58:
#line 350 "syntax/tjs.y"
    { cc->InitLocalVariable(
											  lx->GetString(yyvsp[-2].num), yyvsp[0].np); ;}
    break;

  case 59:
#line 356 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[0].num),
											  ctFunction);
											  cc->EnterBlock();;}
    break;

  case 60:
#line 361 "syntax/tjs.y"
    { cc->ExitBlock(); sb->PopContextStack(); ;}
    break;

  case 61:
#line 366 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(anonymous)"),
											  ctExprFunction);
											  cc->EnterBlock(); ;}
    break;

  case 62:
#line 371 "syntax/tjs.y"
    { cc->ExitBlock();
											  tTJSVariant v(cc);
											  sb->PopContextStack();
											  yyval.np = cc->MakeNP0(T_CONSTVAL);
											  yyval.np->SetValue(v); ;}
    break;

  case 71:
#line 398 "syntax/tjs.y"
    { cc->AddFunctionDeclArg(
												lx->GetString(yyvsp[0].num), NULL); ;}
    break;

  case 72:
#line 400 "syntax/tjs.y"
    { cc->AddFunctionDeclArg(
												lx->GetString(yyvsp[-2].num), yyvsp[0].np); ;}
    break;

  case 73:
#line 405 "syntax/tjs.y"
    { cc->AddFunctionDeclArgCollapse(
												NULL); ;}
    break;

  case 74:
#line 407 "syntax/tjs.y"
    { cc->AddFunctionDeclArgCollapse(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 75:
#line 419 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[-1].num),
												ctProperty); ;}
    break;

  case 76:
#line 423 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 79:
#line 432 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(setter)"),
												ctPropertySetter);
											  cc->EnterBlock();
											  cc->SetPropertyDeclArg(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 80:
#line 438 "syntax/tjs.y"
    { cc->ExitBlock();
	  										  sb->PopContextStack(); ;}
    break;

  case 81:
#line 440 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(getter)"),
												ctPropertyGetter);
											  cc->EnterBlock(); ;}
    break;

  case 82:
#line 444 "syntax/tjs.y"
    { cc->ExitBlock();
	  										  sb->PopContextStack(); ;}
    break;

  case 85:
#line 456 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[0].num),
												ctClass); ;}
    break;

  case 86:
#line 460 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 88:
#line 465 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[0].np, true); ;}
    break;

  case 89:
#line 466 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[-1].np, false); ;}
    break;

  case 93:
#line 476 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[0].np, false); ;}
    break;

  case 94:
#line 481 "syntax/tjs.y"
    { cc->ReturnFromFunc(NULL); ;}
    break;

  case 95:
#line 482 "syntax/tjs.y"
    { cc->ReturnFromFunc(yyvsp[-1].np); ;}
    break;

  case 96:
#line 489 "syntax/tjs.y"
    { cc->EnterSwitchCode(yyvsp[-1].np); ;}
    break;

  case 97:
#line 490 "syntax/tjs.y"
    { cc->ExitSwitchCode(); ;}
    break;

  case 98:
#line 496 "syntax/tjs.y"
    { cc->EnterWithCode(yyvsp[-1].np); ;}
    break;

  case 99:
#line 497 "syntax/tjs.y"
    { cc->ExitWithCode(); ;}
    break;

  case 100:
#line 502 "syntax/tjs.y"
    { cc->ProcessCaseCode(yyvsp[-1].np); ;}
    break;

  case 101:
#line 503 "syntax/tjs.y"
    { cc->ProcessCaseCode(NULL); ;}
    break;

  case 102:
#line 508 "syntax/tjs.y"
    { cc->EnterTryCode(); ;}
    break;

  case 103:
#line 511 "syntax/tjs.y"
    { cc->ExitTryCode(); ;}
    break;

  case 104:
#line 515 "syntax/tjs.y"
    { cc->EnterCatchCode(NULL); ;}
    break;

  case 105:
#line 516 "syntax/tjs.y"
    { cc->EnterCatchCode(NULL); ;}
    break;

  case 106:
#line 517 "syntax/tjs.y"
    { cc->EnterCatchCode(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 107:
#line 523 "syntax/tjs.y"
    { cc->ProcessThrowCode(yyvsp[-1].np); ;}
    break;

  case 108:
#line 528 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 109:
#line 532 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 110:
#line 533 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_IF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 111:
#line 538 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 112:
#line 539 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_COMMA, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 113:
#line 544 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 114:
#line 545 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SWAP, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 115:
#line 546 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_EQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 116:
#line 547 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_AMPERSANDEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 117:
#line 548 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_VERTLINEEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 118:
#line 549 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_CHEVRONEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 119:
#line 550 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_MINUSEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 120:
#line 551 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PLUSEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 121:
#line 552 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PERCENTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 122:
#line 553 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SLASHEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 123:
#line 554 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_BACKSLASHEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 124:
#line 555 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ASTERISKEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 125:
#line 556 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 126:
#line 557 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALANDEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 127:
#line 558 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RARITHSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 128:
#line 559 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LARITHSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 129:
#line 560 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RBITSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 130:
#line 565 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 131:
#line 568 "syntax/tjs.y"
    { yyval.np = cc->MakeNP3(T_QUESTION, yyvsp[-4].np, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 132:
#line 574 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 133:
#line 575 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALOR, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 134:
#line 579 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 135:
#line 581 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALAND, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 136:
#line 585 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 137:
#line 586 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_VERTLINE, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 138:
#line 590 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 139:
#line 591 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_CHEVRON, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 140:
#line 595 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 141:
#line 596 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_AMPERSAND, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 142:
#line 600 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 143:
#line 601 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_NOTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 144:
#line 602 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_EQUALEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 145:
#line 603 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DISCNOTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 146:
#line 604 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DISCEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 147:
#line 608 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 148:
#line 609 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 149:
#line 610 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_GT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 150:
#line 611 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LTOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 151:
#line 612 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_GTOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 152:
#line 616 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 153:
#line 617 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RARITHSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 154:
#line 618 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LARITHSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 155:
#line 619 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RBITSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 156:
#line 624 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 157:
#line 625 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PLUS, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 158:
#line 626 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_MINUS, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 159:
#line 630 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 160:
#line 631 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PERCENT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 161:
#line 632 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SLASH, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 162:
#line 633 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_BACKSLASH, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 163:
#line 634 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ASTERISK, yyvsp[-1].np, yyvsp[0].np); ;}
    break;

  case 164:
#line 638 "syntax/tjs.y"
    { yyval.np = yyvsp[-1].np; ;}
    break;

  case 165:
#line 642 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 166:
#line 643 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXCRAMATION, yyvsp[0].np); ;}
    break;

  case 167:
#line 644 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_TILDE, yyvsp[0].np); ;}
    break;

  case 168:
#line 645 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DECREMENT, yyvsp[0].np); ;}
    break;

  case 169:
#line 646 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INCREMENT, yyvsp[0].np); ;}
    break;

  case 170:
#line 647 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; yyval.np->SetOpecode(T_NEW); ;}
    break;

  case 171:
#line 648 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INVALIDATE, yyvsp[0].np); ;}
    break;

  case 172:
#line 649 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ISVALID, yyvsp[0].np); ;}
    break;

  case 173:
#line 650 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ISVALID, yyvsp[-1].np); ;}
    break;

  case 174:
#line 651 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DELETE, yyvsp[0].np); ;}
    break;

  case 175:
#line 652 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_TYPEOF, yyvsp[0].np); ;}
    break;

  case 176:
#line 653 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_SHARP, yyvsp[0].np); ;}
    break;

  case 177:
#line 654 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DOLLAR, yyvsp[0].np); ;}
    break;

  case 178:
#line 655 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_UPLUS, yyvsp[0].np); ;}
    break;

  case 179:
#line 656 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_UMINUS, yyvsp[0].np); ;}
    break;

  case 180:
#line 657 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_SUBSTANCE, yyvsp[0].np); ;}
    break;

  case 181:
#line 658 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_INSTANCEOF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 182:
#line 659 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INT, yyvsp[0].np); ;}
    break;

  case 183:
#line 660 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INT, yyvsp[0].np); ;}
    break;

  case 184:
#line 661 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_REAL, yyvsp[0].np); ;}
    break;

  case 185:
#line 662 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_REAL, yyvsp[0].np); ;}
    break;

  case 186:
#line 663 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_STRING, yyvsp[0].np); ;}
    break;

  case 187:
#line 664 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_STRING, yyvsp[0].np); ;}
    break;

  case 188:
#line 668 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 189:
#line 670 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_INCONTEXTOF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 190:
#line 674 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 191:
#line 675 "syntax/tjs.y"
    { yyval.np = yyvsp[-1].np; ;}
    break;

  case 192:
#line 676 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LBRACKET, yyvsp[-3].np, yyvsp[-1].np); ;}
    break;

  case 193:
#line 677 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 194:
#line 678 "syntax/tjs.y"
    { lx->SetNextIsBareWord(); ;}
    break;

  case 195:
#line 679 "syntax/tjs.y"
    { tTJSExprNode * node = cc->MakeNP0(T_CONSTVAL);
												  node->SetValue(lx->GetValue(yyvsp[0].num));
												  yyval.np = cc->MakeNP2(T_DOT, yyvsp[-3].np, node); ;}
    break;

  case 196:
#line 682 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_POSTINCREMENT, yyvsp[-1].np); ;}
    break;

  case 197:
#line 683 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_POSTDECREMENT, yyvsp[-1].np); ;}
    break;

  case 198:
#line 684 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EVAL, yyvsp[-1].np); ;}
    break;

  case 199:
#line 685 "syntax/tjs.y"
    { lx->SetNextIsBareWord(); ;}
    break;

  case 200:
#line 686 "syntax/tjs.y"
    { tTJSExprNode * node = cc->MakeNP0(T_CONSTVAL);
												  node->SetValue(lx->GetValue(yyvsp[0].num));
												  yyval.np = cc->MakeNP1(T_WITHDOT, node); ;}
    break;

  case 201:
#line 693 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_CONSTVAL);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 202:
#line 695 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_SYMBOL);
												  yyval.np->SetValue(tTJSVariant(
													lx->GetString(yyvsp[0].num))); ;}
    break;

  case 203:
#line 698 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_THIS); ;}
    break;

  case 204:
#line 699 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_SUPER); ;}
    break;

  case 205:
#line 700 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 206:
#line 701 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_GLOBAL); ;}
    break;

  case 207:
#line 702 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_VOID); ;}
    break;

  case 208:
#line 703 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 209:
#line 704 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 210:
#line 705 "syntax/tjs.y"
    { lx->SetStartOfRegExp(); ;}
    break;

  case 211:
#line 706 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_REGEXP);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 212:
#line 708 "syntax/tjs.y"
    { lx->SetStartOfRegExp(); ;}
    break;

  case 213:
#line 709 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_REGEXP);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 214:
#line 716 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LPARENTHESIS, yyvsp[-3].np, yyvsp[-1].np); ;}
    break;

  case 215:
#line 721 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_OMIT); ;}
    break;

  case 216:
#line 722 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARG, yyvsp[0].np); ;}
    break;

  case 217:
#line 723 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ARG, yyvsp[0].np, yyvsp[-2].np); ;}
    break;

  case 218:
#line 727 "syntax/tjs.y"
    { yyval.np = NULL; ;}
    break;

  case 219:
#line 728 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXPANDARG, NULL); ;}
    break;

  case 220:
#line 729 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXPANDARG, yyvsp[0].np); ;}
    break;

  case 221:
#line 730 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 222:
#line 736 "syntax/tjs.y"
    { tTJSExprNode *node =
										  cc->MakeNP0(T_INLINEARRAY);
										  cc->PushCurrentNode(node); ;}
    break;

  case 223:
#line 740 "syntax/tjs.y"
    { yyval.np = cn; cc->PopCurrentNode(); ;}
    break;

  case 224:
#line 745 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 225:
#line 746 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 226:
#line 751 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARRAYARG, NULL); ;}
    break;

  case 227:
#line 752 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARRAYARG, yyvsp[0].np); ;}
    break;

  case 228:
#line 757 "syntax/tjs.y"
    { tTJSExprNode *node =
										  cc->MakeNP0(T_INLINEDIC);
										  cc->PushCurrentNode(node); ;}
    break;

  case 229:
#line 762 "syntax/tjs.y"
    { yyval.np = cn; cc->PopCurrentNode(); ;}
    break;

  case 231:
#line 769 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 232:
#line 770 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 233:
#line 775 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DICELM, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 234:
#line 776 "syntax/tjs.y"
    { tTJSVariant val(lx->GetString(yyvsp[-2].num));
										  tTJSExprNode *node0 = cc->MakeNP0(T_CONSTVAL);
										  node0->SetValue(val);
										  yyval.np = cc->MakeNP2(T_DICELM, node0, yyvsp[0].np); ;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2904 "tjs.tab.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 789 "syntax/tjs.y"



}
