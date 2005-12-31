#include "tjsCommHead.h"
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
     T_IGNOREPROP = 375,
     T_PROPACCESS = 376,
     T_ARG = 377,
     T_EXPANDARG = 378,
     T_INLINEARRAY = 379,
     T_ARRAYARG = 380,
     T_INLINEDIC = 381,
     T_DICELM = 382,
     T_REGEXP = 383,
     T_WITHDOT = 384,
     T_THIS_PROXY = 385,
     T_WITHDOT_PROXY = 386,
     T_CONSTVAL = 387
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
#define T_IGNOREPROP 375
#define T_PROPACCESS 376
#define T_ARG 377
#define T_EXPANDARG 378
#define T_INLINEARRAY 379
#define T_ARRAYARG 380
#define T_INLINEDIC 381
#define T_DICELM 382
#define T_REGEXP 383
#define T_WITHDOT 384
#define T_THIS_PROXY 385
#define T_WITHDOT_PROXY 386
#define T_CONSTVAL 387




/* Copy the first part of user declarations.  */
#line 1 "syntax/tjs.y"

/*---------------------------------------------------------------------------*/
/*
	TJS2 Script Engine
	Copyright (C) 2000-2006 W.Dee <dee@kikyou.info> and contributors

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
#line 396 "tjs.tab.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 408 "tjs.tab.c"

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
#define YYLAST   1382

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  98
/* YYNRULES -- Number of rules. */
#define YYNRULES  239
/* YYNRULES -- Number of states. */
#define YYNSTATES  405

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   387

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
     125,   126,   127,   128,   129,   130,   131,   132
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
     186,   190,   192,   196,   198,   201,   202,   209,   211,   213,
     216,   219,   220,   227,   228,   232,   236,   238,   239,   245,
     246,   249,   250,   256,   258,   262,   264,   267,   271,   272,
     279,   280,   287,   291,   294,   295,   301,   303,   307,   312,
     316,   318,   320,   324,   326,   330,   332,   336,   340,   344,
     348,   352,   356,   360,   364,   368,   372,   376,   380,   384,
     388,   392,   396,   398,   404,   406,   410,   412,   416,   418,
     422,   424,   428,   430,   434,   436,   440,   444,   448,   452,
     454,   458,   462,   466,   470,   472,   476,   480,   484,   486,
     490,   494,   496,   500,   504,   508,   511,   514,   516,   519,
     522,   525,   528,   531,   534,   537,   540,   543,   546,   549,
     552,   555,   558,   561,   564,   568,   573,   576,   581,   584,
     589,   592,   594,   598,   600,   604,   609,   611,   612,   617,
     620,   623,   626,   627,   631,   633,   635,   637,   639,   641,
     643,   645,   647,   649,   650,   654,   655,   659,   664,   666,
     668,   672,   673,   675,   677,   679,   680,   685,   687,   691,
     692,   694,   695,   702,   703,   705,   709,   713,   717,   718
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
     134,     0,    -1,   135,    -1,    -1,   136,   137,    -1,    -1,
     137,   138,    -1,   137,     1,    65,    -1,   139,    -1,   140,
      -1,    65,    -1,   196,    65,    -1,   148,    -1,   151,    -1,
     142,    -1,   145,    -1,   153,    -1,    81,    65,    -1,    68,
      65,    -1,    70,    65,    -1,   158,    -1,   162,    -1,   171,
      -1,   179,    -1,   185,    -1,   186,    -1,   188,    -1,   190,
      -1,   191,    -1,   194,    -1,    -1,    66,   141,   137,    67,
      -1,    -1,    -1,    88,   143,    55,   196,    63,   144,   138,
      -1,    -1,    -1,    89,   146,   138,    88,    55,   196,    63,
     147,    65,    -1,    -1,    -1,    90,    55,   149,   196,   150,
      63,   138,    -1,    -1,   148,    99,   152,   138,    -1,    87,
      55,   154,    65,   156,    65,   157,    63,   138,    -1,    -1,
      -1,   155,   159,    -1,   196,    -1,    -1,   196,    -1,    -1,
     196,    -1,   159,    65,    -1,    91,   160,    -1,    92,   160,
      -1,   161,    -1,   160,     3,   161,    -1,   117,    -1,   117,
       4,   195,    -1,    -1,    69,   117,   163,   166,   140,    -1,
      -1,    69,   165,   166,   140,    -1,    -1,    55,   170,    63,
      -1,    55,   167,    63,    -1,    55,   168,     3,   170,    63,
      -1,    -1,   168,    -1,   169,    -1,   168,     3,   169,    -1,
     117,    -1,   117,     4,   195,    -1,    40,    -1,   117,    40,
      -1,    -1,    75,   117,    66,   172,   173,    67,    -1,   174,
      -1,   176,    -1,   174,   176,    -1,   176,   174,    -1,    -1,
      97,    55,   117,    63,   175,   140,    -1,    -1,   178,   177,
     140,    -1,    98,    55,    63,    -1,    98,    -1,    -1,    62,
     117,   180,   181,   140,    -1,    -1,    73,   195,    -1,    -1,
      73,   195,     3,   182,   183,    -1,   184,    -1,   183,     3,
     184,    -1,   195,    -1,    80,    65,    -1,    80,   196,    65,
      -1,    -1,    84,    55,   196,    63,   187,   140,    -1,    -1,
     103,    55,   196,    63,   189,   138,    -1,    72,   196,    64,
      -1,    71,    64,    -1,    -1,    96,   192,   138,   193,   138,
      -1,   100,    -1,   100,    55,    63,    -1,   100,    55,   117,
      63,    -1,    95,   196,    65,    -1,   198,    -1,   197,    -1,
     197,    90,   196,    -1,   198,    -1,   197,     3,   198,    -1,
     199,    -1,   199,    29,   198,    -1,   199,     4,   198,    -1,
     199,     5,   198,    -1,   199,     6,   198,    -1,   199,     7,
     198,    -1,   199,     8,   198,    -1,   199,     9,   198,    -1,
     199,    10,   198,    -1,   199,    11,   198,    -1,   199,    12,
     198,    -1,   199,    13,   198,    -1,   199,    14,   198,    -1,
     199,    15,   198,    -1,   199,    18,   198,    -1,   199,    17,
     198,    -1,   199,    16,   198,    -1,   200,    -1,   200,    19,
     199,    64,   199,    -1,   201,    -1,   200,    20,   201,    -1,
     202,    -1,   201,    21,   202,    -1,   203,    -1,   202,    22,
     203,    -1,   204,    -1,   203,    23,   204,    -1,   205,    -1,
     204,    24,   205,    -1,   206,    -1,   205,    25,   206,    -1,
     205,    26,   206,    -1,   205,    27,   206,    -1,   205,    28,
     206,    -1,   207,    -1,   206,    30,   207,    -1,   206,    31,
     207,    -1,   206,    32,   207,    -1,   206,    33,   207,    -1,
     208,    -1,   207,    34,   208,    -1,   207,    35,   208,    -1,
     207,    36,   208,    -1,   209,    -1,   208,    48,   209,    -1,
     208,    49,   209,    -1,   211,    -1,   209,    37,   211,    -1,
     209,    38,   211,    -1,   209,    39,   211,    -1,   210,   211,
      -1,   209,    40,    -1,   212,    -1,    41,   211,    -1,    42,
     211,    -1,    43,   211,    -1,    44,   211,    -1,    45,   219,
      -1,    53,   211,    -1,    52,   211,    -1,   212,    52,    -1,
      46,   211,    -1,    47,   211,    -1,    50,   211,    -1,    51,
     211,    -1,    48,   211,    -1,    49,   211,    -1,    24,   211,
      -1,    40,   211,    -1,   212,    54,   211,    -1,    55,   104,
      63,   211,    -1,   104,   211,    -1,    55,   105,    63,   211,
      -1,   105,   211,    -1,    55,   106,    63,   211,    -1,   106,
     211,    -1,   213,    -1,   213,    86,   212,    -1,   216,    -1,
      55,   196,    63,    -1,   213,    57,   196,    61,    -1,   219,
      -1,    -1,   213,    56,   214,   117,    -1,   213,    44,    -1,
     213,    43,    -1,   213,    41,    -1,    -1,    56,   215,   117,
      -1,   132,    -1,   117,    -1,    58,    -1,    59,    -1,   164,
      -1,    60,    -1,   111,    -1,   222,    -1,   226,    -1,    -1,
      11,   217,   128,    -1,    -1,    38,   218,   128,    -1,   213,
      55,   220,    63,    -1,   101,    -1,   221,    -1,   220,     3,
     221,    -1,    -1,    40,    -1,   210,    -1,   195,    -1,    -1,
      57,   223,   224,    61,    -1,   225,    -1,   224,     3,   225,
      -1,    -1,   195,    -1,    -1,    37,    57,   227,   228,   230,
      61,    -1,    -1,   229,    -1,   228,     3,   229,    -1,   195,
       3,   195,    -1,   117,    64,   195,    -1,    -1,     3,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   213,   213,   218,   218,   224,   226,   227,   234,   235,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     264,   264,   271,   272,   271,   278,   281,   278,   287,   288,
     287,   294,   294,   300,   310,   311,   311,   313,   319,   320,
     325,   326,   331,   335,   336,   343,   344,   349,   351,   357,
     357,   367,   367,   380,   382,   383,   384,   388,   390,   394,
     395,   399,   401,   406,   408,   420,   419,   428,   429,   430,
     431,   435,   435,   446,   446,   455,   456,   462,   462,   469,
     471,   472,   472,   477,   478,   482,   487,   488,   495,   494,
     502,   501,   508,   509,   514,   514,   521,   522,   523,   529,
     534,   538,   539,   544,   545,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   571,   572,   580,   581,   585,   586,   591,   592,
     596,   597,   601,   602,   606,   607,   608,   609,   610,   614,
     615,   616,   617,   618,   622,   623,   624,   625,   630,   631,
     632,   636,   637,   638,   639,   640,   644,   648,   649,   650,
     651,   652,   653,   654,   655,   656,   657,   658,   659,   660,
     661,   662,   663,   664,   665,   666,   667,   668,   669,   670,
     671,   675,   676,   681,   682,   683,   684,   685,   685,   689,
     690,   691,   692,   692,   700,   702,   705,   706,   707,   708,
     709,   710,   711,   712,   712,   715,   715,   723,   728,   729,
     730,   734,   735,   736,   737,   743,   743,   752,   753,   758,
     759,   764,   764,   774,   776,   777,   782,   783,   790,   792
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
  "T_IGNOREPROP", "T_PROPACCESS", "T_ARG", "T_EXPANDARG", "T_INLINEARRAY", 
  "T_ARRAYARG", "T_INLINEDIC", "T_DICELM", "T_REGEXP", "T_WITHDOT", 
  "T_THIS_PROXY", "T_WITHDOT_PROXY", "T_CONSTVAL", "$accept", "program", 
  "global_list", "@1", "def_list", "block_or_statement", "statement", 
  "block", "@2", "while", "@3", "@4", "do_while", "@5", "@6", "if", "@7", 
  "@8", "if_else", "@9", "for", "for_first_clause", "@10", 
  "for_second_clause", "for_third_clause", "variable_def", 
  "variable_def_inner", "variable_id_list", "variable_id", "func_def", 
  "@11", "func_expr_def", "@12", "func_decl_arg_opt", 
  "func_decl_arg_list", "func_decl_arg_at_least_one", "func_decl_arg", 
  "func_decl_arg_collapse", "property_def", "@13", 
  "property_handler_def_list", "property_handler_setter", "@14", 
  "property_handler_getter", "@15", "property_getter_handler_head", 
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
     385,   386,   387
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   133,   134,   136,   135,   137,   137,   137,   138,   138,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   139,
     141,   140,   143,   144,   142,   146,   147,   145,   149,   150,
     148,   152,   151,   153,   154,   155,   154,   154,   156,   156,
     157,   157,   158,   159,   159,   160,   160,   161,   161,   163,
     162,   165,   164,   166,   166,   166,   166,   167,   167,   168,
     168,   169,   169,   170,   170,   172,   171,   173,   173,   173,
     173,   175,   174,   177,   176,   178,   178,   180,   179,   181,
     181,   182,   181,   183,   183,   184,   185,   185,   187,   186,
     189,   188,   190,   190,   192,   191,   193,   193,   193,   194,
     195,   196,   196,   197,   197,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   199,   199,   200,   200,   201,   201,   202,   202,
     203,   203,   204,   204,   205,   205,   205,   205,   205,   206,
     206,   206,   206,   206,   207,   207,   207,   207,   208,   208,
     208,   209,   209,   209,   209,   209,   210,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   212,   212,   213,   213,   213,   213,   214,   213,   213,
     213,   213,   215,   213,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   217,   216,   218,   216,   219,   220,   220,
     220,   221,   221,   221,   221,   223,   222,   224,   224,   225,
     225,   227,   226,   228,   228,   228,   229,   229,   230,   230
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
       3,     1,     3,     1,     2,     0,     6,     1,     1,     2,
       2,     0,     6,     0,     3,     3,     1,     0,     5,     0,
       2,     0,     5,     1,     3,     1,     2,     3,     0,     6,
       0,     6,     3,     2,     0,     5,     1,     3,     4,     3,
       1,     1,     3,     1,     3,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     5,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     2,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     4,     2,     4,     2,     4,
       2,     1,     3,     1,     3,     4,     1,     0,     4,     2,
       2,     2,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     0,     3,     4,     1,     1,
       3,     0,     1,     1,     1,     0,     4,     1,     3,     0,
       1,     0,     6,     0,     1,     3,     3,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       3,     0,     2,     5,     1,     0,     0,   213,     0,     0,
     215,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,   225,   206,   207,
     209,     0,    10,    30,     0,    61,     0,     0,     0,     0,
       0,     0,     0,     0,    32,    35,     0,     0,     0,     0,
     104,     0,     0,     0,     0,   210,   205,   204,     6,     8,
       9,    14,    15,    12,    13,    16,    20,     0,    21,   208,
      22,    23,    24,    25,    26,    27,    28,    29,     0,   111,
     113,   115,   132,   134,   136,   138,   140,   142,   144,   149,
     154,   158,     0,   161,   167,   191,   193,   196,   211,   212,
       7,     0,    61,   182,   231,     0,   183,   168,   169,   170,
     171,     0,     0,   172,   176,   177,   180,   181,   178,   179,
     174,   173,     0,     0,     0,     0,     0,   229,    87,     5,
      18,    59,    63,    19,   103,     0,     0,    96,     0,    17,
       0,    45,     0,     0,    38,    57,    53,    55,    54,     0,
       0,     0,   186,   188,   190,    41,    52,    11,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   166,   165,
     175,     0,   201,   200,   199,   221,   197,     0,     0,   214,
     233,   216,     0,     0,     0,   194,   203,   230,   110,     0,
     227,    89,     0,    63,    67,     0,   102,    75,    97,     0,
       0,     0,    47,     0,     0,     0,     0,     0,   109,     0,
       0,     0,   114,   112,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   131,   130,   129,   116,
       0,   135,   137,   139,   141,   143,   145,   146,   147,   148,
     150,   151,   152,   153,   155,   156,   157,   159,   160,   162,
     163,   164,   184,   222,   218,   224,   223,     0,   219,     0,
       0,   192,   205,     0,   238,   234,   185,   187,   189,   229,
     226,     0,     0,    31,     0,    73,    71,     0,    68,    69,
       0,    62,     0,    98,    48,    46,     0,     0,    39,    58,
      56,   106,     0,   100,    42,     0,   221,   217,   198,   195,
       0,     0,   239,     0,   228,    90,    88,    60,     0,    74,
      65,     0,    64,     0,    86,     0,    77,    78,    83,     0,
       0,    49,    33,     0,     0,     0,   105,     0,   133,   220,
     237,   236,   235,   232,    91,    72,    70,     0,     0,     0,
      76,    79,    80,     0,    99,    50,     0,     0,     0,   107,
       0,   101,     0,    66,     0,    85,    84,     0,    51,    34,
      36,    40,   108,    92,    93,    95,    81,     0,     0,     0,
       0,    43,    37,    94,    82
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     1,     2,     3,     5,    58,    59,    60,   129,    61,
     142,   376,    62,   143,   398,    63,   235,   354,    64,   241,
      65,   230,   231,   350,   387,    66,    67,   146,   147,    68,
     223,    69,   132,   225,   307,   308,   309,   310,    70,   312,
     345,   346,   400,   347,   373,   348,    71,   221,   302,   382,
     393,   394,    72,    73,   349,    74,   357,    75,    76,   150,
     322,    77,   217,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,   289,   126,    96,   101,   105,    97,   287,   288,    98,
     127,   219,   220,    99,   210,   294,   295,   333
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -204
static const short yypact[] =
{
    -204,    18,  -204,  -204,  -204,   338,   -30,  -204,  1016,    25,
    -204,  1016,  1016,  1016,  1016,  1016,    52,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1094,  -204,  -204,  -204,  -204,
    -204,   -53,  -204,  -204,    41,    -3,    48,    53,  1016,     2,
     201,    57,    70,    78,  -204,  -204,    82,    42,    42,  1016,
    -204,   105,  1016,  1016,  1016,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,    62,  -204,  -204,  -204,    97,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,    99,     1,
    -204,   692,    32,   145,   146,   144,   147,    19,    40,    23,
     -10,    64,  1016,  -204,   -40,    -7,  -204,  -204,  -204,  -204,
    -204,    44,  -204,  -204,  -204,    45,  -204,  -204,  -204,  -204,
    -204,  1016,    -1,    37,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,   626,   704,   782,   107,    58,  1016,  -204,  -204,
    -204,  -204,   119,  -204,  -204,   112,   111,  -204,   113,  -204,
    1016,   860,   126,   530,  -204,   181,   183,  -204,   183,   122,
     530,  1016,  -204,  -204,  -204,  -204,  -204,  -204,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  -204,  -204,
    -204,  1016,  -204,  -204,  -204,   938,  -204,  1016,    52,  -204,
    1172,  -204,  1016,  1016,  1016,  -204,  -204,  -204,  -204,     7,
    -204,   117,   434,   119,   -34,   125,  -204,  -204,  -204,   129,
     128,    -4,  -204,  1016,   106,  1016,  1016,    42,  -204,    95,
     134,   530,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
     135,   145,   146,   144,   147,    19,    40,    40,    40,    40,
      23,    23,    23,    23,   -10,   -10,   -10,    64,    64,  -204,
    -204,  -204,  -204,  1016,  -204,  -204,  1016,     6,  -204,    81,
     139,  -204,   137,   199,   203,  -204,  -204,  -204,  -204,  1016,
    -204,  1016,   125,  -204,   125,  -204,    13,   148,   204,  -204,
     150,  -204,   -13,  -204,  1016,  -204,   154,   163,  -204,  -204,
    -204,   164,   530,  -204,  -204,  1016,  1250,  -204,  -204,  -204,
    1016,  1016,  1172,   162,  -204,   221,  -204,  -204,  1016,  -204,
    -204,   -34,  -204,   171,   172,   161,   131,   133,  -204,   125,
     166,  -204,  -204,  1016,   169,   -52,  -204,   530,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,   170,   118,   174,
    -204,  -204,  -204,   125,  -204,  1016,   530,   177,   530,  -204,
     200,  -204,  1016,  -204,   202,  -204,  -204,   205,  -204,  -204,
    -204,  -204,  -204,   259,  -204,  -204,  -204,   530,   206,  1016,
     125,  -204,  -204,  -204,  -204
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -204,  -204,  -204,  -204,   138,  -142,  -204,  -184,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,    33,   224,    36,  -204,
    -204,  -204,  -204,    46,  -204,  -204,   -67,   -66,  -204,  -204,
    -204,   -71,  -204,   -68,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -119,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -203,   -25,  -204,   -22,  -171,  -204,   104,   108,
     103,   110,   102,  -108,   -31,   -95,  -127,  -202,     8,    76,
     269,  -204,  -204,  -204,  -204,  -204,   271,  -204,   -38,  -204,
    -204,  -204,    -6,  -204,  -204,  -204,   -37,  -204
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -197
static const short yytable[] =
{
     125,   234,   285,   286,   158,   260,   305,   293,   239,   326,
     299,   379,   200,   135,   201,   138,   103,   338,     4,   106,
     107,   108,   109,   110,   149,   114,   115,   116,   117,   118,
     119,   120,   121,   319,   202,   100,   203,   204,   193,   194,
     202,   311,   203,   204,   182,   183,   184,   185,   205,   206,
     207,   176,   177,   339,   205,   206,   207,   190,   191,   192,
     152,   153,   154,     7,   128,   380,   277,   278,   300,   327,
     186,   187,   188,   189,   266,   267,   268,   269,  -196,   208,
    -196,  -196,   104,   306,   343,   344,   125,    47,    48,     9,
      10,   159,  -196,  -196,  -196,   274,   275,   276,   335,   324,
     199,   195,   196,   197,   198,   218,   130,   111,    26,    27,
      28,    29,    30,   133,   131,   229,   232,   134,   336,   136,
     337,   102,   139,   285,   286,   140,   240,   360,   361,   293,
     152,   153,   154,   141,   243,   365,   242,   144,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   358,   270,   271,   272,   273,   145,
     151,   155,   156,    55,   157,   374,   178,   180,   179,    56,
     215,   181,   209,   211,   224,   216,   226,   227,   228,   395,
     356,   233,   290,   218,    57,   236,   237,   238,   218,   386,
     301,    33,   313,   314,   317,   321,   395,   323,   328,   325,
     329,   330,   331,   279,   280,   281,   332,   341,   316,   282,
     318,   340,     7,   342,   218,   381,   404,   352,   353,   355,
     296,   297,   298,   363,   364,     8,   368,   369,   370,   344,
     343,   375,   378,   383,   389,   384,   391,   385,     9,    10,
     390,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,   401,    25,    26,    27,    28,
      29,    30,   399,   392,   315,   396,   137,   222,   397,   304,
     102,   402,   148,   320,   366,   367,   372,   218,   371,   218,
     403,   261,   263,   265,   291,   112,   262,   113,   359,   351,
     264,   106,     0,   334,   199,   362,     0,     0,     0,     0,
       0,     0,     0,     0,   218,    52,    53,    54,   218,   218,
     218,     0,    55,     0,     0,     0,   218,     0,    56,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   377,     0,
       0,     0,     0,    57,     0,     0,     0,     0,    -4,     6,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
     388,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     218,     0,     8,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     9,    10,   218,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,     0,    25,    26,    27,    28,    29,    30,     0,
      31,     0,     0,    32,    33,     0,    34,    35,    36,    37,
      38,     0,     0,    39,     0,     0,     0,     0,    40,    41,
       0,     0,    42,     0,     0,    43,    44,    45,    46,    47,
      48,     0,     0,    49,    50,     6,     0,     0,     0,     0,
       0,    51,    52,    53,    54,     7,     0,     0,     0,    55,
       0,     0,     0,     0,     0,    56,     0,     0,     8,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,     9,    10,     0,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,     0,    25,
      26,    27,    28,    29,    30,     0,    31,     0,     0,    32,
      33,   303,    34,    35,    36,    37,    38,     0,     0,    39,
       0,     0,     0,     0,    40,    41,     0,     0,    42,     0,
       0,    43,    44,    45,    46,    47,    48,     0,     0,    49,
      50,     0,     0,     0,     0,     0,     0,    51,    52,    53,
      54,     7,     0,     0,     0,    55,     0,     0,     0,     0,
       0,    56,     0,     0,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57,     9,    10,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,     0,    25,    26,    27,    28,    29,
      30,     0,    31,     0,     0,    32,    33,     0,    34,    35,
      36,    37,    38,     0,     0,    39,     0,     0,     0,     0,
      40,    41,     0,     0,    42,     0,     0,    43,    44,    45,
      46,    47,    48,     0,     0,    49,    50,     0,     0,     0,
       0,     0,     0,    51,    52,    53,    54,     7,     0,     0,
       0,    55,     0,     0,     0,     0,     0,    56,     0,     0,
       8,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    57,     9,    10,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
       0,    25,    26,    27,    28,    29,    30,     0,     0,   212,
       0,     0,     0,     0,     0,   102,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       0,   175,     0,     0,     0,     0,     0,     0,     8,     0,
      52,    53,    54,     0,     0,     0,     0,    55,     0,     0,
       0,     9,    10,    56,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    57,    25,
      26,    27,    28,    29,    30,     0,     0,   213,     0,     0,
       0,     0,     0,   102,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,     0,    52,    53,
      54,     0,     0,     0,     0,    55,     0,     0,     0,     9,
      10,    56,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    57,    25,    26,    27,
      28,    29,    30,     0,     0,   214,     0,     0,     0,     0,
       0,   102,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     8,     0,    52,    53,    54,     0,
       0,     0,     0,    55,     0,     0,     0,     9,    10,    56,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    57,    25,    26,    27,    28,    29,
      30,     0,     0,     0,     0,   -44,     0,     0,     0,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     8,     0,    52,    53,    54,     0,     0,     0,
       0,    55,     0,     0,     0,     9,    10,    56,   283,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    57,    25,    26,    27,    28,    29,    30,     0,
       0,     0,     0,     0,     0,     0,     0,   102,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   284,
       8,     0,    52,    53,    54,     0,     0,     0,     0,    55,
       0,     0,     0,     9,    10,    56,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      57,    25,    26,    27,    28,    29,    30,     0,     0,     0,
       0,     0,     0,     0,     0,   102,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     8,     0,
      52,    53,    54,     0,     0,     0,     0,    55,     0,     0,
       0,     9,    10,    56,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    57,    25,
      26,    27,    28,    29,    30,     0,     0,     0,     0,     0,
       0,     0,     0,   102,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,     0,   122,   123,
     124,     0,     0,     0,     0,    55,     0,     0,     0,     9,
      10,    56,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    57,    25,    26,    27,
      28,    29,    30,     0,     0,     0,     0,     0,     0,     0,
       0,   102,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     8,     0,    52,    53,    54,     0,
       0,     0,     0,    55,     0,     0,     0,     9,    10,   292,
     283,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    57,    25,    26,    27,    28,    29,
      30,     0,     0,     0,     0,     0,     0,     0,     0,   102,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    52,    53,    54,     0,     0,     0,
       0,    55,     0,     0,     0,     0,     0,    56,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    57
};

static const short yycheck[] =
{
      25,   143,   205,   205,     3,   176,    40,   210,   150,     3,
       3,    63,    52,    38,    54,    40,     8,     4,     0,    11,
      12,    13,    14,    15,    49,    17,    18,    19,    20,    21,
      22,    23,    24,   236,    41,    65,    43,    44,    48,    49,
      41,   225,    43,    44,    25,    26,    27,    28,    55,    56,
      57,    19,    20,    40,    55,    56,    57,    34,    35,    36,
      52,    53,    54,    11,   117,   117,   193,   194,    61,    63,
      30,    31,    32,    33,   182,   183,   184,   185,    41,    86,
      43,    44,    57,   117,    97,    98,   111,    91,    92,    37,
      38,    90,    55,    56,    57,   190,   191,   192,   301,   241,
      92,    37,    38,    39,    40,   127,    65,    55,    56,    57,
      58,    59,    60,    65,   117,   140,   141,    64,   302,   117,
     304,    69,    65,   326,   326,    55,   151,   330,   331,   332,
     122,   123,   124,    55,   159,   338,   158,    55,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   325,   186,   187,   188,   189,   117,
      55,    99,    65,   111,    65,   349,    21,    23,    22,   117,
      63,    24,   128,   128,    55,   117,    64,    66,    65,   382,
     322,    55,   207,   205,   132,     4,     3,    65,   210,   373,
      73,    66,    63,    65,    88,   100,   399,    63,   117,    64,
      61,    64,     3,   195,   196,   197,     3,     3,   233,   201,
     235,    63,    11,    63,   236,   357,   400,    63,    55,    55,
     212,   213,   214,    61,     3,    24,    55,    55,    67,    98,
      97,    65,    63,    63,   376,   117,   378,    63,    37,    38,
      63,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,   397,    55,    56,    57,    58,
      59,    60,     3,    63,   231,    63,    65,   129,    63,   223,
      69,    65,    48,   237,   341,   341,   347,   299,   346,   301,
     399,   177,   179,   181,   208,    16,   178,    16,   326,   314,
     180,   283,    -1,   299,   286,   332,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   326,   104,   105,   106,   330,   331,
     332,    -1,   111,    -1,    -1,    -1,   338,    -1,   117,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   353,    -1,
      -1,    -1,    -1,   132,    -1,    -1,    -1,    -1,     0,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
     375,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     382,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    38,   399,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    55,    56,    57,    58,    59,    60,    -1,
      62,    -1,    -1,    65,    66,    -1,    68,    69,    70,    71,
      72,    -1,    -1,    75,    -1,    -1,    -1,    -1,    80,    81,
      -1,    -1,    84,    -1,    -1,    87,    88,    89,    90,    91,
      92,    -1,    -1,    95,    96,     1,    -1,    -1,    -1,    -1,
      -1,   103,   104,   105,   106,    11,    -1,    -1,    -1,   111,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     132,    37,    38,    -1,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    55,
      56,    57,    58,    59,    60,    -1,    62,    -1,    -1,    65,
      66,    67,    68,    69,    70,    71,    72,    -1,    -1,    75,
      -1,    -1,    -1,    -1,    80,    81,    -1,    -1,    84,    -1,
      -1,    87,    88,    89,    90,    91,    92,    -1,    -1,    95,
      96,    -1,    -1,    -1,    -1,    -1,    -1,   103,   104,   105,
     106,    11,    -1,    -1,    -1,   111,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   132,    37,    38,    -1,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    55,    56,    57,    58,    59,
      60,    -1,    62,    -1,    -1,    65,    66,    -1,    68,    69,
      70,    71,    72,    -1,    -1,    75,    -1,    -1,    -1,    -1,
      80,    81,    -1,    -1,    84,    -1,    -1,    87,    88,    89,
      90,    91,    92,    -1,    -1,    95,    96,    -1,    -1,    -1,
      -1,    -1,    -1,   103,   104,   105,   106,    11,    -1,    -1,
      -1,   111,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   132,    37,    38,    -1,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    58,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    29,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
     104,   105,   106,    -1,    -1,    -1,    -1,   111,    -1,    -1,
      -1,    37,    38,   117,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,   132,    55,
      56,    57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,   104,   105,
     106,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,    37,
      38,   117,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,   132,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    -1,   104,   105,   106,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,    37,    38,   117,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,   132,    55,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    24,    -1,   104,   105,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,    37,    38,   117,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,   132,    55,    56,    57,    58,    59,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      24,    -1,   104,   105,   106,    -1,    -1,    -1,    -1,   111,
      -1,    -1,    -1,    37,    38,   117,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
     132,    55,    56,    57,    58,    59,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,
     104,   105,   106,    -1,    -1,    -1,    -1,   111,    -1,    -1,
      -1,    37,    38,   117,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,   132,    55,
      56,    57,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    24,    -1,   104,   105,
     106,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,    37,
      38,   117,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,   132,    55,    56,    57,
      58,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    24,    -1,   104,   105,   106,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,    37,    38,   117,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,   132,    55,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   104,   105,   106,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   132
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,   134,   135,   136,     0,   137,     1,    11,    24,    37,
      38,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    55,    56,    57,    58,    59,
      60,    62,    65,    66,    68,    69,    70,    71,    72,    75,
      80,    81,    84,    87,    88,    89,    90,    91,    92,    95,
      96,   103,   104,   105,   106,   111,   117,   132,   138,   139,
     140,   142,   145,   148,   151,   153,   158,   159,   162,   164,
     171,   179,   185,   186,   188,   190,   191,   194,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   216,   219,   222,   226,
      65,   217,    69,   211,    57,   218,   211,   211,   211,   211,
     211,    55,   213,   219,   211,   211,   211,   211,   211,   211,
     211,   211,   104,   105,   106,   196,   215,   223,   117,   141,
      65,   117,   165,    65,    64,   196,   117,    65,   196,    65,
      55,    55,   143,   146,    55,   117,   160,   161,   160,   196,
     192,    55,   211,   211,   211,    99,    65,    65,     3,    90,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    29,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    30,    31,    32,    33,
      34,    35,    36,    48,    49,    37,    38,    39,    40,   211,
      52,    54,    41,    43,    44,    55,    56,    57,    86,   128,
     227,   128,    63,    63,    63,    63,   117,   195,   198,   224,
     225,   180,   137,   163,    55,   166,    64,    66,    65,   196,
     154,   155,   196,    55,   138,   149,     4,     3,    65,   138,
     196,   152,   198,   196,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     199,   201,   202,   203,   204,   205,   206,   206,   206,   206,
     207,   207,   207,   207,   208,   208,   208,   209,   209,   211,
     211,   211,   211,    40,   101,   195,   210,   220,   221,   214,
     196,   212,   117,   195,   228,   229,   211,   211,   211,     3,
      61,    73,   181,    67,   166,    40,   117,   167,   168,   169,
     170,   140,   172,    63,    65,   159,   196,    88,   196,   195,
     161,   100,   193,    63,   138,    64,     3,    63,   117,    61,
      64,     3,     3,   230,   225,   195,   140,   140,     4,    40,
      63,     3,    63,    97,    98,   173,   174,   176,   178,   187,
     156,   196,    63,    55,   150,    55,   138,   189,   199,   221,
     195,   195,   229,    61,     3,   195,   169,   170,    55,    55,
      67,   176,   174,   177,   140,    65,   144,   196,    63,    63,
     117,   138,   182,    63,   117,    63,   140,   157,   196,   138,
      63,   138,    63,   183,   184,   195,    63,    63,   147,     3,
     175,   138,    65,   184,   140
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
#line 218 "syntax/tjs.y"
    { sb->PushContextStack(TJS_W("global"),
												ctTopLevel); ;}
    break;

  case 4:
#line 220 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 7:
#line 227 "syntax/tjs.y"
    { if(sb->CompileErrorCount>20)
												YYABORT;
											  else yyerrok; ;}
    break;

  case 11:
#line 241 "syntax/tjs.y"
    { cc->CreateExprCode(yyvsp[-1].np); ;}
    break;

  case 17:
#line 247 "syntax/tjs.y"
    { cc->DoBreak(); ;}
    break;

  case 18:
#line 248 "syntax/tjs.y"
    { cc->DoContinue(); ;}
    break;

  case 19:
#line 249 "syntax/tjs.y"
    { cc->DoDebugger(); ;}
    break;

  case 30:
#line 264 "syntax/tjs.y"
    { cc->EnterBlock(); ;}
    break;

  case 31:
#line 266 "syntax/tjs.y"
    { cc->ExitBlock(); ;}
    break;

  case 32:
#line 271 "syntax/tjs.y"
    { cc->EnterWhileCode(false); ;}
    break;

  case 33:
#line 272 "syntax/tjs.y"
    { cc->CreateWhileExprCode(yyvsp[-1].np, false); ;}
    break;

  case 34:
#line 273 "syntax/tjs.y"
    { cc->ExitWhileCode(false); ;}
    break;

  case 35:
#line 278 "syntax/tjs.y"
    { cc->EnterWhileCode(true); ;}
    break;

  case 36:
#line 281 "syntax/tjs.y"
    { cc->CreateWhileExprCode(yyvsp[-1].np, true); ;}
    break;

  case 37:
#line 282 "syntax/tjs.y"
    { cc->ExitWhileCode(true); ;}
    break;

  case 38:
#line 287 "syntax/tjs.y"
    { cc->EnterIfCode(); ;}
    break;

  case 39:
#line 288 "syntax/tjs.y"
    { cc->CreateIfExprCode(yyvsp[0].np); ;}
    break;

  case 40:
#line 289 "syntax/tjs.y"
    { cc->ExitIfCode(); ;}
    break;

  case 41:
#line 294 "syntax/tjs.y"
    { cc->EnterElseCode(); ;}
    break;

  case 42:
#line 295 "syntax/tjs.y"
    { cc->ExitElseCode(); ;}
    break;

  case 43:
#line 304 "syntax/tjs.y"
    { cc->ExitForCode(); ;}
    break;

  case 44:
#line 310 "syntax/tjs.y"
    { cc->EnterForCode(false); ;}
    break;

  case 45:
#line 311 "syntax/tjs.y"
    { cc->EnterForCode(true); ;}
    break;

  case 47:
#line 313 "syntax/tjs.y"
    { cc->EnterForCode(false);
											  cc->CreateExprCode(yyvsp[0].np); ;}
    break;

  case 48:
#line 319 "syntax/tjs.y"
    { cc->CreateForExprCode(NULL); ;}
    break;

  case 49:
#line 320 "syntax/tjs.y"
    { cc->CreateForExprCode(yyvsp[0].np); ;}
    break;

  case 50:
#line 325 "syntax/tjs.y"
    { cc->SetForThirdExprCode(NULL); ;}
    break;

  case 51:
#line 326 "syntax/tjs.y"
    { cc->SetForThirdExprCode(yyvsp[0].np); ;}
    break;

  case 57:
#line 349 "syntax/tjs.y"
    { cc->AddLocalVariable(
												lx->GetString(yyvsp[0].num)); ;}
    break;

  case 58:
#line 351 "syntax/tjs.y"
    { cc->InitLocalVariable(
											  lx->GetString(yyvsp[-2].num), yyvsp[0].np); ;}
    break;

  case 59:
#line 357 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[0].num),
											  ctFunction);
											  cc->EnterBlock();;}
    break;

  case 60:
#line 362 "syntax/tjs.y"
    { cc->ExitBlock(); sb->PopContextStack(); ;}
    break;

  case 61:
#line 367 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(anonymous)"),
											  ctExprFunction);
											  cc->EnterBlock(); ;}
    break;

  case 62:
#line 372 "syntax/tjs.y"
    { cc->ExitBlock();
											  tTJSVariant v(cc);
											  sb->PopContextStack();
											  yyval.np = cc->MakeNP0(T_CONSTVAL);
											  yyval.np->SetValue(v); ;}
    break;

  case 71:
#line 399 "syntax/tjs.y"
    { cc->AddFunctionDeclArg(
												lx->GetString(yyvsp[0].num), NULL); ;}
    break;

  case 72:
#line 401 "syntax/tjs.y"
    { cc->AddFunctionDeclArg(
												lx->GetString(yyvsp[-2].num), yyvsp[0].np); ;}
    break;

  case 73:
#line 406 "syntax/tjs.y"
    { cc->AddFunctionDeclArgCollapse(
												NULL); ;}
    break;

  case 74:
#line 408 "syntax/tjs.y"
    { cc->AddFunctionDeclArgCollapse(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 75:
#line 420 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[-1].num),
												ctProperty); ;}
    break;

  case 76:
#line 424 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 81:
#line 435 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(setter)"),
												ctPropertySetter);
											  cc->EnterBlock();
											  cc->SetPropertyDeclArg(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 82:
#line 441 "syntax/tjs.y"
    { cc->ExitBlock();
											  sb->PopContextStack(); ;}
    break;

  case 83:
#line 446 "syntax/tjs.y"
    { sb->PushContextStack(
												TJS_W("(getter)"),
												ctPropertyGetter);
											  cc->EnterBlock(); ;}
    break;

  case 84:
#line 450 "syntax/tjs.y"
    { cc->ExitBlock();
											  sb->PopContextStack(); ;}
    break;

  case 87:
#line 462 "syntax/tjs.y"
    { sb->PushContextStack(
												lx->GetString(yyvsp[0].num),
												ctClass); ;}
    break;

  case 88:
#line 466 "syntax/tjs.y"
    { sb->PopContextStack(); ;}
    break;

  case 90:
#line 471 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[0].np, true); ;}
    break;

  case 91:
#line 472 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[-1].np, false); ;}
    break;

  case 95:
#line 482 "syntax/tjs.y"
    { cc->CreateExtendsExprCode(yyvsp[0].np, false); ;}
    break;

  case 96:
#line 487 "syntax/tjs.y"
    { cc->ReturnFromFunc(NULL); ;}
    break;

  case 97:
#line 488 "syntax/tjs.y"
    { cc->ReturnFromFunc(yyvsp[-1].np); ;}
    break;

  case 98:
#line 495 "syntax/tjs.y"
    { cc->EnterSwitchCode(yyvsp[-1].np); ;}
    break;

  case 99:
#line 496 "syntax/tjs.y"
    { cc->ExitSwitchCode(); ;}
    break;

  case 100:
#line 502 "syntax/tjs.y"
    { cc->EnterWithCode(yyvsp[-1].np); ;}
    break;

  case 101:
#line 503 "syntax/tjs.y"
    { cc->ExitWithCode(); ;}
    break;

  case 102:
#line 508 "syntax/tjs.y"
    { cc->ProcessCaseCode(yyvsp[-1].np); ;}
    break;

  case 103:
#line 509 "syntax/tjs.y"
    { cc->ProcessCaseCode(NULL); ;}
    break;

  case 104:
#line 514 "syntax/tjs.y"
    { cc->EnterTryCode(); ;}
    break;

  case 105:
#line 517 "syntax/tjs.y"
    { cc->ExitTryCode(); ;}
    break;

  case 106:
#line 521 "syntax/tjs.y"
    { cc->EnterCatchCode(NULL); ;}
    break;

  case 107:
#line 522 "syntax/tjs.y"
    { cc->EnterCatchCode(NULL); ;}
    break;

  case 108:
#line 523 "syntax/tjs.y"
    { cc->EnterCatchCode(
												lx->GetString(yyvsp[-1].num)); ;}
    break;

  case 109:
#line 529 "syntax/tjs.y"
    { cc->ProcessThrowCode(yyvsp[-1].np); ;}
    break;

  case 110:
#line 534 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 111:
#line 538 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 112:
#line 539 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_IF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 113:
#line 544 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 114:
#line 545 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_COMMA, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 115:
#line 550 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 116:
#line 551 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SWAP, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 117:
#line 552 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_EQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 118:
#line 553 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_AMPERSANDEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 119:
#line 554 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_VERTLINEEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 120:
#line 555 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_CHEVRONEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 121:
#line 556 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_MINUSEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 122:
#line 557 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PLUSEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 123:
#line 558 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PERCENTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 124:
#line 559 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SLASHEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 125:
#line 560 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_BACKSLASHEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 126:
#line 561 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ASTERISKEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 127:
#line 562 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 128:
#line 563 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALANDEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 129:
#line 564 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RARITHSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 130:
#line 565 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LARITHSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 131:
#line 566 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RBITSHIFTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 132:
#line 571 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 133:
#line 574 "syntax/tjs.y"
    { yyval.np = cc->MakeNP3(T_QUESTION, yyvsp[-4].np, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 134:
#line 580 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 135:
#line 581 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALOR, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 136:
#line 585 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 137:
#line 587 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LOGICALAND, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 138:
#line 591 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 139:
#line 592 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_VERTLINE, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 140:
#line 596 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 141:
#line 597 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_CHEVRON, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 142:
#line 601 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 143:
#line 602 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_AMPERSAND, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 144:
#line 606 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 145:
#line 607 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_NOTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 146:
#line 608 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_EQUALEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 147:
#line 609 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DISCNOTEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 148:
#line 610 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DISCEQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 149:
#line 614 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 150:
#line 615 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 151:
#line 616 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_GT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 152:
#line 617 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LTOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 153:
#line 618 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_GTOREQUAL, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 154:
#line 622 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 155:
#line 623 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RARITHSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 156:
#line 624 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LARITHSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 157:
#line 625 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_RBITSHIFT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 158:
#line 630 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 159:
#line 631 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PLUS, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 160:
#line 632 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_MINUS, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 161:
#line 636 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 162:
#line 637 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_PERCENT, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 163:
#line 638 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_SLASH, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 164:
#line 639 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_BACKSLASH, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 165:
#line 640 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ASTERISK, yyvsp[-1].np, yyvsp[0].np); ;}
    break;

  case 166:
#line 644 "syntax/tjs.y"
    { yyval.np = yyvsp[-1].np; ;}
    break;

  case 167:
#line 648 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 168:
#line 649 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXCRAMATION, yyvsp[0].np); ;}
    break;

  case 169:
#line 650 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_TILDE, yyvsp[0].np); ;}
    break;

  case 170:
#line 651 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DECREMENT, yyvsp[0].np); ;}
    break;

  case 171:
#line 652 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INCREMENT, yyvsp[0].np); ;}
    break;

  case 172:
#line 653 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; yyval.np->SetOpecode(T_NEW); ;}
    break;

  case 173:
#line 654 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INVALIDATE, yyvsp[0].np); ;}
    break;

  case 174:
#line 655 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ISVALID, yyvsp[0].np); ;}
    break;

  case 175:
#line 656 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ISVALID, yyvsp[-1].np); ;}
    break;

  case 176:
#line 657 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DELETE, yyvsp[0].np); ;}
    break;

  case 177:
#line 658 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_TYPEOF, yyvsp[0].np); ;}
    break;

  case 178:
#line 659 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_SHARP, yyvsp[0].np); ;}
    break;

  case 179:
#line 660 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_DOLLAR, yyvsp[0].np); ;}
    break;

  case 180:
#line 661 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_UPLUS, yyvsp[0].np); ;}
    break;

  case 181:
#line 662 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_UMINUS, yyvsp[0].np); ;}
    break;

  case 182:
#line 663 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_IGNOREPROP, yyvsp[0].np); ;}
    break;

  case 183:
#line 664 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_PROPACCESS, yyvsp[0].np); ;}
    break;

  case 184:
#line 665 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_INSTANCEOF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 185:
#line 666 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INT, yyvsp[0].np); ;}
    break;

  case 186:
#line 667 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_INT, yyvsp[0].np); ;}
    break;

  case 187:
#line 668 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_REAL, yyvsp[0].np); ;}
    break;

  case 188:
#line 669 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_REAL, yyvsp[0].np); ;}
    break;

  case 189:
#line 670 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_STRING, yyvsp[0].np); ;}
    break;

  case 190:
#line 671 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_STRING, yyvsp[0].np); ;}
    break;

  case 191:
#line 675 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 192:
#line 677 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_INCONTEXTOF, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 193:
#line 681 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 194:
#line 682 "syntax/tjs.y"
    { yyval.np = yyvsp[-1].np; ;}
    break;

  case 195:
#line 683 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LBRACKET, yyvsp[-3].np, yyvsp[-1].np); ;}
    break;

  case 196:
#line 684 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 197:
#line 685 "syntax/tjs.y"
    { lx->SetNextIsBareWord(); ;}
    break;

  case 198:
#line 686 "syntax/tjs.y"
    { tTJSExprNode * node = cc->MakeNP0(T_CONSTVAL);
												  node->SetValue(lx->GetValue(yyvsp[0].num));
												  yyval.np = cc->MakeNP2(T_DOT, yyvsp[-3].np, node); ;}
    break;

  case 199:
#line 689 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_POSTINCREMENT, yyvsp[-1].np); ;}
    break;

  case 200:
#line 690 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_POSTDECREMENT, yyvsp[-1].np); ;}
    break;

  case 201:
#line 691 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EVAL, yyvsp[-1].np); ;}
    break;

  case 202:
#line 692 "syntax/tjs.y"
    { lx->SetNextIsBareWord(); ;}
    break;

  case 203:
#line 693 "syntax/tjs.y"
    { tTJSExprNode * node = cc->MakeNP0(T_CONSTVAL);
												  node->SetValue(lx->GetValue(yyvsp[0].num));
												  yyval.np = cc->MakeNP1(T_WITHDOT, node); ;}
    break;

  case 204:
#line 700 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_CONSTVAL);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 205:
#line 702 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_SYMBOL);
												  yyval.np->SetValue(tTJSVariant(
													lx->GetString(yyvsp[0].num))); ;}
    break;

  case 206:
#line 705 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_THIS); ;}
    break;

  case 207:
#line 706 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_SUPER); ;}
    break;

  case 208:
#line 707 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 209:
#line 708 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_GLOBAL); ;}
    break;

  case 210:
#line 709 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_VOID); ;}
    break;

  case 211:
#line 710 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 212:
#line 711 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 213:
#line 712 "syntax/tjs.y"
    { lx->SetStartOfRegExp(); ;}
    break;

  case 214:
#line 713 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_REGEXP);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 215:
#line 715 "syntax/tjs.y"
    { lx->SetStartOfRegExp(); ;}
    break;

  case 216:
#line 716 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_REGEXP);
												  yyval.np->SetValue(lx->GetValue(yyvsp[0].num)); ;}
    break;

  case 217:
#line 723 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_LPARENTHESIS, yyvsp[-3].np, yyvsp[-1].np); ;}
    break;

  case 218:
#line 728 "syntax/tjs.y"
    { yyval.np = cc->MakeNP0(T_OMIT); ;}
    break;

  case 219:
#line 729 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARG, yyvsp[0].np); ;}
    break;

  case 220:
#line 730 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_ARG, yyvsp[0].np, yyvsp[-2].np); ;}
    break;

  case 221:
#line 734 "syntax/tjs.y"
    { yyval.np = NULL; ;}
    break;

  case 222:
#line 735 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXPANDARG, NULL); ;}
    break;

  case 223:
#line 736 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_EXPANDARG, yyvsp[0].np); ;}
    break;

  case 224:
#line 737 "syntax/tjs.y"
    { yyval.np = yyvsp[0].np; ;}
    break;

  case 225:
#line 743 "syntax/tjs.y"
    { tTJSExprNode *node =
										  cc->MakeNP0(T_INLINEARRAY);
										  cc->PushCurrentNode(node); ;}
    break;

  case 226:
#line 747 "syntax/tjs.y"
    { yyval.np = cn; cc->PopCurrentNode(); ;}
    break;

  case 227:
#line 752 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 228:
#line 753 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 229:
#line 758 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARRAYARG, NULL); ;}
    break;

  case 230:
#line 759 "syntax/tjs.y"
    { yyval.np = cc->MakeNP1(T_ARRAYARG, yyvsp[0].np); ;}
    break;

  case 231:
#line 764 "syntax/tjs.y"
    { tTJSExprNode *node =
										  cc->MakeNP0(T_INLINEDIC);
										  cc->PushCurrentNode(node); ;}
    break;

  case 232:
#line 769 "syntax/tjs.y"
    { yyval.np = cn; cc->PopCurrentNode(); ;}
    break;

  case 234:
#line 776 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 235:
#line 777 "syntax/tjs.y"
    { cn->Add(yyvsp[0].np); ;}
    break;

  case 236:
#line 782 "syntax/tjs.y"
    { yyval.np = cc->MakeNP2(T_DICELM, yyvsp[-2].np, yyvsp[0].np); ;}
    break;

  case 237:
#line 783 "syntax/tjs.y"
    { tTJSVariant val(lx->GetString(yyvsp[-2].num));
										  tTJSExprNode *node0 = cc->MakeNP0(T_CONSTVAL);
										  node0->SetValue(val);
										  yyval.np = cc->MakeNP2(T_DICELM, node0, yyvsp[0].np); ;}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2916 "tjs.tab.c"

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


#line 796 "syntax/tjs.y"



}
