/* A Bison parser, made by GNU Bison 3.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "../src/parser.y" /* yacc.c:339  */

//----------------------------------------------------------------------------
// parser.y:
//
// InstallerLG parser
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "all.h"
#include "alloc.h"
#include "eval.h"
#include "init.h"
#include "lexer.h"

#include <string.h>

// Always debug.
#define YYDEBUG 1

#line 88 "parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SYM = 258,
    STR = 259,
    OOM = 260,
    INT = 261,
    HEX = 262,
    BIN = 263,
    AND = 264,
    BITAND = 265,
    BITNOT = 266,
    BITOR = 267,
    BITXOR = 268,
    NOT = 269,
    IN = 270,
    OR = 271,
    SHIFTLEFT = 272,
    SHIFTRIGHT = 273,
    XOR = 274,
    EQ = 275,
    GT = 276,
    GTE = 277,
    LT = 278,
    LTE = 279,
    NEQ = 280,
    IF = 281,
    SELECT = 282,
    UNTIL = 283,
    WHILE = 284,
    TRACE = 285,
    RETRACE = 286,
    EXECUTE = 287,
    REXX = 288,
    RUN = 289,
    ABORT = 290,
    EXIT = 291,
    ONERROR = 292,
    TRAP = 293,
    REBOOT = 294,
    COPYFILES = 295,
    COPYLIB = 296,
    DELETE = 297,
    EXISTS = 298,
    FILEONLY = 299,
    FOREACH = 300,
    MAKEASSIGN = 301,
    MAKEDIR = 302,
    PROTECT = 303,
    STARTUP = 304,
    TEXTFILE = 305,
    TOOLTYPE = 306,
    TRANSCRIPT = 307,
    RENAME = 308,
    COMPLETE = 309,
    DEBUG = 310,
    MESSAGE = 311,
    USER = 312,
    WELCOME = 313,
    WORKING = 314,
    DATABASE = 315,
    EARLIER = 316,
    GETASSIGN = 317,
    GETDEVICE = 318,
    GETDISKSPACE = 319,
    GETENV = 320,
    GETSIZE = 321,
    GETSUM = 322,
    GETVERSION = 323,
    ICONINFO = 324,
    CUS = 325,
    DCL = 326,
    ASKBOOL = 327,
    ASKCHOICE = 328,
    ASKDIR = 329,
    ASKDISK = 330,
    ASKFILE = 331,
    ASKNUMBER = 332,
    ASKOPTIONS = 333,
    ASKSTRING = 334,
    CAT = 335,
    EXPANDPATH = 336,
    FMT = 337,
    PATHONLY = 338,
    PATMATCH = 339,
    STRLEN = 340,
    SUBSTR = 341,
    TACKON = 342,
    SET = 343,
    SYMBOLSET = 344,
    SYMBOLVAL = 345,
    OPENWBOBJECT = 346,
    SHOWWBOBJECT = 347,
    CLOSEWBOBJECT = 348,
    ALL = 349,
    APPEND = 350,
    ASSIGNS = 351,
    BACK = 352,
    CHOICES = 353,
    COMMAND = 354,
    COMPRESSION = 355,
    CONFIRM = 356,
    DEFAULT = 357,
    DELOPTS = 358,
    DEST = 359,
    DISK = 360,
    FILES = 361,
    FONTS = 362,
    GETDEFAULTTOOL = 363,
    GETPOSITION = 364,
    GETSTACK = 365,
    GETTOOLTYPE = 366,
    HELP = 367,
    INFOS = 368,
    INCLUDE = 369,
    NEWNAME = 370,
    NEWPATH = 371,
    NOGAUGE = 372,
    NOPOSITION = 373,
    NOREQ = 374,
    PATTERN = 375,
    PROMPT = 376,
    QUIET = 377,
    RANGE = 378,
    SAFE = 379,
    SETDEFAULTTOOL = 380,
    SETPOSITION = 381,
    SETSTACK = 382,
    SETTOOLTYPE = 383,
    SOURCE = 384,
    SWAPCOLORS = 385,
    OPTIONAL = 386,
    RESIDENT = 387,
    OVERRIDE = 388
  };
#endif
/* Tokens.  */
#define SYM 258
#define STR 259
#define OOM 260
#define INT 261
#define HEX 262
#define BIN 263
#define AND 264
#define BITAND 265
#define BITNOT 266
#define BITOR 267
#define BITXOR 268
#define NOT 269
#define IN 270
#define OR 271
#define SHIFTLEFT 272
#define SHIFTRIGHT 273
#define XOR 274
#define EQ 275
#define GT 276
#define GTE 277
#define LT 278
#define LTE 279
#define NEQ 280
#define IF 281
#define SELECT 282
#define UNTIL 283
#define WHILE 284
#define TRACE 285
#define RETRACE 286
#define EXECUTE 287
#define REXX 288
#define RUN 289
#define ABORT 290
#define EXIT 291
#define ONERROR 292
#define TRAP 293
#define REBOOT 294
#define COPYFILES 295
#define COPYLIB 296
#define DELETE 297
#define EXISTS 298
#define FILEONLY 299
#define FOREACH 300
#define MAKEASSIGN 301
#define MAKEDIR 302
#define PROTECT 303
#define STARTUP 304
#define TEXTFILE 305
#define TOOLTYPE 306
#define TRANSCRIPT 307
#define RENAME 308
#define COMPLETE 309
#define DEBUG 310
#define MESSAGE 311
#define USER 312
#define WELCOME 313
#define WORKING 314
#define DATABASE 315
#define EARLIER 316
#define GETASSIGN 317
#define GETDEVICE 318
#define GETDISKSPACE 319
#define GETENV 320
#define GETSIZE 321
#define GETSUM 322
#define GETVERSION 323
#define ICONINFO 324
#define CUS 325
#define DCL 326
#define ASKBOOL 327
#define ASKCHOICE 328
#define ASKDIR 329
#define ASKDISK 330
#define ASKFILE 331
#define ASKNUMBER 332
#define ASKOPTIONS 333
#define ASKSTRING 334
#define CAT 335
#define EXPANDPATH 336
#define FMT 337
#define PATHONLY 338
#define PATMATCH 339
#define STRLEN 340
#define SUBSTR 341
#define TACKON 342
#define SET 343
#define SYMBOLSET 344
#define SYMBOLVAL 345
#define OPENWBOBJECT 346
#define SHOWWBOBJECT 347
#define CLOSEWBOBJECT 348
#define ALL 349
#define APPEND 350
#define ASSIGNS 351
#define BACK 352
#define CHOICES 353
#define COMMAND 354
#define COMPRESSION 355
#define CONFIRM 356
#define DEFAULT 357
#define DELOPTS 358
#define DEST 359
#define DISK 360
#define FILES 361
#define FONTS 362
#define GETDEFAULTTOOL 363
#define GETPOSITION 364
#define GETSTACK 365
#define GETTOOLTYPE 366
#define HELP 367
#define INFOS 368
#define INCLUDE 369
#define NEWNAME 370
#define NEWPATH 371
#define NOGAUGE 372
#define NOPOSITION 373
#define NOREQ 374
#define PATTERN 375
#define PROMPT 376
#define QUIET 377
#define RANGE 378
#define SAFE 379
#define SETDEFAULTTOOL 380
#define SETPOSITION 381
#define SETSTACK 382
#define SETTOOLTYPE 383
#define SOURCE 384
#define SWAPCOLORS 385
#define OPTIONAL 386
#define RESIDENT 387
#define OVERRIDE 388

/* Value type.  */



int yyparse (yyscan_t scanner);

#endif /* !YY_YY_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 399 "parser.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  187
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2002

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  143
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  149
/* YYNRULES -- Number of rules.  */
#define YYNRULES  354
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  684

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   388

#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     134,   135,   138,   136,     2,   139,     2,   137,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     142,   140,   141,     2,     2,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   131,   132,   133
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    99,    99,   100,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   271,   272,   273,   274,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   290,   291,   292,   293,   294,   295,   298,   299,   300,
     301,   302,   303,   304,   305,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   322,   323,   324,
     325,   326,   327,   328,   330,   331,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   350,   351,   352,   353,   354,   355,   356,   357,
     358,   359,   360,   361,   362,   363,   364,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   377,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     392,   393,   394,   397,   398,   399,   400,   401,   402,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,   432,   433,   434,   437,   438,   439,   440,   441,
     442,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   471,   472,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     484,   485,   486,   487,   488
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYM", "STR", "OOM", "INT", "HEX", "BIN",
  "AND", "BITAND", "BITNOT", "BITOR", "BITXOR", "NOT", "IN", "OR",
  "SHIFTLEFT", "SHIFTRIGHT", "XOR", "EQ", "GT", "GTE", "LT", "LTE", "NEQ",
  "IF", "SELECT", "UNTIL", "WHILE", "TRACE", "RETRACE", "EXECUTE", "REXX",
  "RUN", "ABORT", "EXIT", "ONERROR", "TRAP", "REBOOT", "COPYFILES",
  "COPYLIB", "DELETE", "EXISTS", "FILEONLY", "FOREACH", "MAKEASSIGN",
  "MAKEDIR", "PROTECT", "STARTUP", "TEXTFILE", "TOOLTYPE", "TRANSCRIPT",
  "RENAME", "COMPLETE", "DEBUG", "MESSAGE", "USER", "WELCOME", "WORKING",
  "DATABASE", "EARLIER", "GETASSIGN", "GETDEVICE", "GETDISKSPACE",
  "GETENV", "GETSIZE", "GETSUM", "GETVERSION", "ICONINFO", "CUS", "DCL",
  "ASKBOOL", "ASKCHOICE", "ASKDIR", "ASKDISK", "ASKFILE", "ASKNUMBER",
  "ASKOPTIONS", "ASKSTRING", "CAT", "EXPANDPATH", "FMT", "PATHONLY",
  "PATMATCH", "STRLEN", "SUBSTR", "TACKON", "SET", "SYMBOLSET",
  "SYMBOLVAL", "OPENWBOBJECT", "SHOWWBOBJECT", "CLOSEWBOBJECT", "ALL",
  "APPEND", "ASSIGNS", "BACK", "CHOICES", "COMMAND", "COMPRESSION",
  "CONFIRM", "DEFAULT", "DELOPTS", "DEST", "DISK", "FILES", "FONTS",
  "GETDEFAULTTOOL", "GETPOSITION", "GETSTACK", "GETTOOLTYPE", "HELP",
  "INFOS", "INCLUDE", "NEWNAME", "NEWPATH", "NOGAUGE", "NOPOSITION",
  "NOREQ", "PATTERN", "PROMPT", "QUIET", "RANGE", "SAFE", "SETDEFAULTTOOL",
  "SETPOSITION", "SETSTACK", "SETTOOLTYPE", "SOURCE", "SWAPCOLORS",
  "OPTIONAL", "RESIDENT", "OVERRIDE", "'('", "')'", "'+'", "'/'", "'*'",
  "'-'", "'='", "'>'", "'<'", "$accept", "start", "s", "p", "pp", "ps",
  "pps", "vp", "vps", "opts", "xpb", "xpbs", "np", "sps", "par", "cv",
  "cvv", "opt", "ivp", "add", "div", "mul", "sub", "and", "bitand",
  "bitnot", "bitor", "bitxor", "not", "in", "or", "shiftleft",
  "shiftright", "xor", "eq", "gt", "gte", "lt", "lte", "neq", "if",
  "select", "until", "while", "trace", "retrace", "execute", "rexx", "run",
  "abort", "exit", "onerror", "reboot", "trap", "copyfiles", "copylib",
  "delete", "exists", "fileonly", "foreach", "makeassign", "makedir",
  "protect", "startup", "textfile", "tooltype", "transcript", "rename",
  "complete", "debug", "message", "user", "welcome", "working", "database",
  "earlier", "getassign", "getdevice", "getdiskspace", "getenv", "getsize",
  "getsum", "getversion", "iconinfo", "dcl", "cus", "askbool", "askchoice",
  "askdir", "askdisk", "askfile", "asknumber", "askoptions", "askstring",
  "cat", "expandpath", "fmt", "pathonly", "patmatch", "strlen", "substr",
  "tackon", "set", "symbolset", "symbolval", "openwbobject",
  "showwbobject", "closewbobject", "all", "append", "assigns", "back",
  "choices", "command", "compression", "confirm", "default", "delopts",
  "dest", "disk", "files", "fonts", "getdefaulttool", "getposition",
  "getstack", "gettooltype", "help", "infos", "include", "newname",
  "newpath", "nogauge", "noposition", "noreq", "pattern", "prompt",
  "quiet", "range", "safe", "setdefaulttool", "setposition", "setstack",
  "settooltype", "source", "swapcolors", "optional", "resident",
  "override", "dynopt", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
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
     385,   386,   387,   388,    40,    41,    43,    47,    42,    45,
      61,    62,    60
};
# endif

#define YYPACT_NINF -543

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-543)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -124,  1517,    19,  -543,  -543,  -124,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,    34,    68,   251,   251,
     251,   251,   251,   251,   251,   251,   251,   251,   251,   251,
     251,   251,   251,   251,   251,   251,  -102,  -100,   340,   340,
     340,   251,    74,  -124,   251,   -85,   -73,   -73,   340,   251,
     251,   251,   251,   340,   251,   340,   -73,   -73,   251,   251,
     251,    85,   340,   251,    93,   251,   251,   251,   251,   251,
     251,   251,   251,   251,   108,   -73,   105,  -126,   -73,  -119,
     -73,  -117,  -111,   -73,  -109,   251,   251,   251,   251,   251,
     251,   251,   116,   251,   251,   340,   251,   251,   251,   251,
     251,   251,   251,   251,   251,   -13,   -91,  -543,  -124,  -543,
    -543,  -543,  -543,  -543,  -543,  1657,  -543,  -543,   128,  -543,
    -543,  -543,   134,   143,   251,   -11,     9,    17,    26,    38,
     251,   150,    41,    50,    59,    64,    66,    83,   207,    86,
     103,   407,  -124,  -124,  -543,  -543,   957,   228,   340,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,   245,   340,   290,   340,   297,  1237,  -543,   303,   309,
     -89,  -124,  -543,  1742,   -87,   -75,   -70,   340,   -68,   110,
    -124,   315,   -40,   -30,   340,   351,   -17,   -73,   398,    -8,
      -6,   484,    37,   112,  -543,   523,   571,   340,   140,  -543,
     602,   608,   686,   144,   147,   692,   156,   164,   725,   192,
     196,   200,   205,  -543,    44,    47,    25,  -543,    49,    63,
    -543,    70,    72,  -543,    82,  -543,    95,   106,  -543,   126,
     745,   216,   218,   225,   237,   761,   261,   251,    31,  -543,
     830,   264,   130,   340,   274,   310,   843,   313,  1337,   316,
     318,   319,   320,  -543,  -543,   -13,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  1343,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  1517,  -543,  -543,   407,  -543,  -543,
    -543,  1349,   132,   136,   251,   321,   251,   322,  -124,   251,
     251,   323,  1357,   251,   251,   251,   324,   325,   326,   251,
     251,   251,   251,  1482,   327,   251,   251,   330,   331,   332,
     359,   251,   251,   360,   251,   361,   251,   251,   251,   251,
     251,   370,   251,   371,   251,   139,  -543,   152,  1490,   -73,
    -543,   154,  1499,  -543,   190,  1623,  -543,  -543,   372,  -543,
    1632,  -543,   195,   251,  -543,  -543,  -543,   199,   203,   391,
    -543,   379,  -543,   215,  1377,  -543,   389,   401,  -543,   399,
    -543,   233,   236,  -543,   240,  -543,   242,   247,  -543,   254,
    -543,  -543,  -543,  -543,   256,  -543,  -543,  -543,   260,  1638,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,   417,  -543,   419,  -543,  -543,  -543,
     423,    29,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,   424,  -543,  -543,   251,
    -543,  -543,   251,  -543,  -543,   273,   282,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  1748,  -543,  -543,
    -543,  -543,  -543,  1756,  -543,  1771,  -543,   284,  1777,  1797,
    -543,  -543,   431,   433,  1803,   434,  -543,  -543,  -543,   435,
     446,   447,   448,  -543,  1809,  -543,   449,   456,  -543,  -543,
    -543,  -543,   457,  1815,  -543,   459,  -543,   460,   462,   463,
    1821,   464,   468,  -543,  1827,  -543,   482,  -543,  -543,  -543,
     286,  -543,  -543,   288,  -543,  -543,   292,  -543,  -543,  -543,
     455,  -543,  -543,   294,  -543,  -543,  -543,  -543,  -543,  -543,
     299,  -543,  -543,  -543,  -543,   301,  -543,  -543,  -543,   305,
    -543,  -543,  -543,  -543,   485,  -543,  -543,  -543,  -543,  -543,
     307,  -543,   486,  1097,   455,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  1869,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,   488,  -543
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     2,    14,     3,    11,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     111,   110,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   143,   142,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     1,    13,    28,
      27,    29,    24,    25,    26,     0,   278,     8,     0,     4,
       5,   295,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   193,   194,     0,     0,     0,    17,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    56,
      55,    57,    58,    59,    60,    61,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    62,    76,    63,
      75,     0,     0,     0,     0,     0,     0,   212,     0,     0,
       0,     0,   214,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   249,     0,     0,     0,     0,   256,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   269,     0,     0,     0,   279,     0,     0,
     282,     0,     0,   285,     0,   287,     0,     0,   290,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,    15,     0,   277,     7,   294,   170,
       6,   171,   172,   173,   174,   175,     0,   177,   178,   179,
     180,   183,   185,   186,     0,   189,    21,    34,   188,   187,
      22,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   198,     0,     0,    16,
     202,     0,     0,   206,     0,     0,   207,   211,     0,   210,
       0,   213,     0,     0,   216,   217,   221,     0,     0,     0,
     222,     0,   224,     0,     0,   229,     0,     0,   227,     0,
     233,     0,     0,   237,     0,   235,     0,     0,   241,     0,
     242,   243,   244,   246,     0,   247,   248,   253,     0,     0,
     254,   255,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,     0,   270,     0,   272,    33,   276,
       0,     0,   280,   281,   283,   284,   286,   288,   289,   291,
     292,   293,   296,   297,   298,   299,     0,   301,    31,     0,
     302,   303,     0,   304,   305,     0,     0,   309,   310,   166,
     167,   168,   169,   181,   182,   184,   176,     0,    35,   190,
      23,   191,   192,     0,   311,     0,   313,     0,     0,     0,
     317,   319,     0,     0,     0,     0,   323,   324,   325,     0,
       0,     0,     0,   331,     0,   332,     0,     0,   335,   336,
     337,   338,     0,     0,   341,     0,   343,     0,     0,     0,
       0,     0,     0,   350,     0,   352,     0,    18,   195,   196,
       0,   199,   200,     0,   203,   204,     0,   208,   209,   215,
       0,   218,   219,     0,   223,   225,   228,   226,   230,   231,
       0,   236,   234,   238,   239,     0,   245,   250,   251,     0,
     271,   275,    32,   274,     0,   300,    30,     9,   306,   307,
       0,    19,     0,     0,     0,   312,   314,   315,   316,   318,
     320,   321,   322,   326,   327,   328,   329,   330,   333,   334,
     339,   340,   342,   344,   345,   346,   348,   347,   349,   351,
     353,   197,   201,   205,     0,   220,   232,   240,   252,   273,
     308,    20,     0,   354
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -543,  -543,  -319,   369,   454,   -35,  -543,     0,     2,  -106,
    -217,  -543,    79,  -543,  -543,  -543,  -543,  -542,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,   341,  -543,  -543,  -116,  -543,   155,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,   304,  -543,  -543
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   197,   205,   198,   350,   199,   188,   439,
     387,   391,   200,   348,   511,   219,   220,   229,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
       4,   185,     5,   186,   390,     4,   279,   510,   283,   327,
       1,   644,   228,   272,   274,   283,   330,   283,   333,   187,
     284,   285,   287,   283,   335,   283,   338,   294,   508,   298,
     299,   300,   632,   224,   529,   225,   307,   189,   190,   191,
     192,   193,   194,     1,   364,     1,   451,   283,   454,   325,
     282,   328,   329,   331,   332,   334,   336,   337,   339,   283,
     455,   283,   202,   203,   283,   456,   459,   460,   644,   353,
     211,   189,   190,   191,   192,   193,   194,   189,   190,   191,
     192,   193,   194,   227,   271,   273,   275,   278,   189,   190,
     191,   192,   193,   194,   467,   468,   189,   190,   191,   192,
     193,   194,   682,   301,   283,   470,   305,   306,   326,   310,
     311,   189,   190,   191,   192,   193,   194,   283,   475,   347,
     435,   437,   363,     4,   371,   280,   283,   480,   283,   481,
     340,   189,   190,   191,   192,   193,   194,   189,   190,   191,
     192,   193,   194,   356,   372,   358,   189,   190,   191,   192,
     193,   194,   373,   189,   190,   191,   192,   193,   194,     1,
     509,   374,   448,     1,   633,   441,   530,   444,   195,   196,
     548,   283,   483,   375,   550,   376,   378,   435,   504,   505,
     457,   283,   507,   283,   512,   379,     4,   471,     4,   474,
     476,   477,   634,   438,   380,   365,   484,   283,   513,   381,
     488,   382,   195,   201,   283,   514,   283,   515,   276,   277,
     189,   190,   191,   192,   193,   194,   283,   516,   383,   195,
     304,   388,     4,     4,   392,   393,   365,   195,   309,   283,
     517,   189,   190,   191,   192,   193,   194,   442,   389,   445,
     283,   518,   195,   323,   450,   462,   535,   485,   189,   190,
     191,   192,   193,   194,   189,   190,   191,   192,   193,   194,
     283,   519,   195,   366,   283,   534,     1,   551,   195,   368,
       1,   552,   489,   283,   597,   490,   365,   195,   369,   494,
       4,     4,   495,   452,   195,   377,   283,   598,   283,   601,
       4,   497,   463,   189,   190,   191,   192,   193,   194,   498,
     189,   190,   191,   192,   193,   194,   189,   190,   191,   192,
     193,   194,   189,   190,   191,   192,   193,   194,   189,   190,
     191,   192,   193,   194,   283,   604,     4,   500,     5,     1,
     609,   501,   600,   283,   611,   502,   603,   283,   612,   606,
     503,   384,   385,   189,   190,   191,   192,   193,   194,     1,
     615,   521,   613,   522,   189,   190,   191,   192,   193,   194,
     523,   555,   226,   436,   558,   559,   620,   283,   618,   564,
     283,   619,   524,   625,   283,   621,   283,   622,   574,   226,
     440,   283,   623,   629,   185,   195,   547,   583,   283,   624,
     283,   626,     4,     4,   283,   627,   527,   594,     4,   533,
     557,   189,   190,   191,   192,   193,   194,   283,   638,   537,
     189,   190,   191,   192,   193,   194,   283,   639,     1,   646,
     283,   671,   283,   672,   226,   443,   283,   673,   283,   675,
     640,   195,   446,   283,   676,   283,   677,   276,   447,   283,
     678,   283,   680,   195,   449,   538,   466,   469,   540,   464,
     465,   542,     4,   543,   544,   545,   554,   556,   560,   566,
     567,   568,   575,     4,   365,   578,   579,   580,   204,   206,
     204,   204,   209,   210,   226,   204,   204,   204,   204,   204,
     204,   218,   221,   222,   223,   226,   473,   189,   190,   191,
     192,   193,   194,   281,   581,   584,   586,   286,   288,   289,
     204,   291,   293,   295,   297,   593,   595,   607,   204,   303,
     420,     4,   308,     5,   614,   312,   204,   315,   317,   318,
     320,   321,   322,   324,   616,   425,   189,   190,   191,   192,
     193,   194,   226,   478,   617,   341,   342,   204,   344,   204,
     204,   384,   204,   351,   352,   354,   355,     4,   204,   433,
     204,   204,   204,   204,   630,   207,   208,     4,   631,   635,
     212,   213,   214,   215,   216,   217,   649,   367,   650,   652,
     653,   367,   367,   370,   189,   190,   191,   192,   193,   194,
     367,   654,   655,   656,   658,   290,   292,   386,   296,   674,
     386,   659,   660,   302,   662,   663,   367,   664,   665,   667,
     313,   314,   316,   668,   319,   189,   190,   191,   192,   193,
     194,   189,   190,   191,   192,   193,   194,   670,   195,   482,
     679,   681,   343,   683,   345,   346,   642,   349,   506,   461,
       0,     0,     0,   357,     0,   359,   360,   361,   362,     0,
     367,     0,   367,   185,   367,   547,     0,   367,     0,     0,
       0,     0,     0,     0,     0,     0,   458,   195,   486,     0,
     370,     0,     0,   472,   370,     0,     0,   479,     0,     0,
     367,     0,     0,     0,   367,   367,     0,     0,     0,   367,
     367,   370,     0,     0,   370,     0,     0,   370,     0,   189,
     190,   191,   192,   193,   194,   189,   190,   191,   192,   193,
     194,     0,     0,     0,     0,   226,   487,     0,     0,   367,
       0,     0,     0,     0,   526,     0,   528,     0,     0,   532,
       0,     0,   536,     0,     0,   367,     0,   367,   189,   190,
     191,   192,   193,   194,     0,     0,   195,   491,     0,     0,
       0,     0,   195,   492,     0,   367,     0,     0,   189,   190,
     191,   192,   193,   194,     0,     0,   386,     0,     0,     0,
     386,     0,     0,   553,   189,   190,   191,   192,   193,   194,
       0,   562,   563,     0,   565,     0,     0,     0,   569,   204,
     571,   204,     0,     0,   576,   577,     0,     0,     0,     0,
     582,     0,     0,   204,     0,   587,   204,   589,   590,   592,
       0,     0,     0,   596,     0,     0,     0,   367,     0,     0,
       0,   367,     0,     0,   367,     0,     0,     0,     0,   367,
     195,   493,   610,     0,     0,     0,   195,   496,     0,     0,
       0,     0,     0,   189,   190,   191,   192,   193,   194,     0,
       0,     0,     0,     0,     0,     0,   189,   190,   191,   192,
     193,   194,     0,     0,     0,     0,     0,     0,   367,   195,
     499,     0,     0,     0,   570,     0,   572,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   585,   195,
     520,   588,     0,   591,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   195,   525,     0,   636,     0,
       0,   637,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   386,     0,   367,     0,     0,   367,   367,     0,
       0,     0,     0,   367,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   367,     0,     0,     0,     0,     0,     0,
       0,     0,   367,     0,     0,     0,     0,     0,     0,   370,
      96,    97,     0,   367,   195,   531,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   195,   539,   109,
       0,   110,   111,   394,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,   426,   427,   428,   429,   430,   431,   432,   433,
     434,   226,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   394,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,   426,   427,   428,   429,   430,   431,   432,   433,
     434,     1,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     189,   190,   191,   192,   193,   194,   189,   190,   191,   192,
     193,   194,   189,   190,   191,   192,   193,   194,     0,   423,
     189,   190,   191,   192,   193,   194,     0,     0,     0,     0,
       0,   195,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   195,   541,     0,     0,     0,     0,   195,   546,     0,
       0,     0,     0,   384,   549,   189,   190,   191,   192,   193,
     194,   195,   561,   189,   190,   191,   192,   193,   194,     0,
       0,   425,   189,   190,   191,   192,   193,   194,     0,     0,
       0,   195,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,     0,     0,     0,     0,     0,   195,   573,     0,     0,
       0,     0,     0,     0,   226,   599,   189,   190,   191,   192,
     193,   194,     0,   226,   602,   189,   190,   191,   192,   193,
     194,   189,   190,   191,   192,   193,   194,     0,     0,     0,
       0,     1,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   189,   190,   191,   192,   193,   194,   226,   605,   189,
     190,   191,   192,   193,   194,     0,   195,   608,   453,     0,
       0,     0,   226,   628,   189,   190,   191,   192,   193,   194,
     189,   190,   191,   192,   193,   194,     0,     0,     0,     0,
       0,   195,     0,   178,   179,   180,   181,   182,   183,   184,
     189,   190,   191,   192,   193,   194,   189,   190,   191,   192,
     193,   194,   189,   190,   191,   192,   193,   194,   189,   190,
     191,   192,   193,   194,   189,   190,   191,   192,   193,   194,
     189,   190,   191,   192,   193,   194,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   407,   408,
     409,   410,   411,   412,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   425,   426,   427,   428,
     429,   430,   431,   432,   433,   434,   283,     0,     0,     0,
       0,     0,     1,   641,     0,     0,     0,     0,     0,     0,
     643,   385,     0,     0,     0,   453,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   195,   645,     0,     0,     0,
       0,   195,   647,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   195,   648,     0,     0,     0,     0,   195,   651,     0,
       0,     0,     0,   195,   657,     0,     0,     0,     0,   195,
     661,     0,     0,     0,     0,   195,   666,     0,     0,     0,
       0,   195,   669,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   409,   410,   411,
     412,   413,   414,   415,   416,   417,   418,   419,   420,   421,
     422,   423,   424,   425,   426,   427,   428,   429,   430,   431,
     432,   433,   434
};

static const yytype_int16 yycheck[] =
{
       0,     1,     0,     1,   221,     5,   122,   326,   134,   135,
     134,   553,   118,   119,   120,   134,   135,   134,   135,     0,
     126,   127,   128,   134,   135,   134,   135,   133,     3,   135,
     136,   137,     3,   135,     3,   135,   142,     3,     4,     5,
       6,     7,     8,   134,   135,   134,   135,   134,   135,   155,
     135,   157,   158,   159,   160,   161,   162,   163,   164,   134,
     135,   134,    97,    98,   134,   135,   134,   135,   610,   175,
     105,     3,     4,     5,     6,     7,     8,     3,     4,     5,
       6,     7,     8,   118,   119,   120,   121,   122,     3,     4,
       5,     6,     7,     8,   134,   135,     3,     4,     5,     6,
       7,     8,   644,   138,   134,   135,   141,   142,     3,   144,
     145,     3,     4,     5,     6,     7,     8,   134,   135,     3,
     226,   227,   135,   123,   135,   123,   134,   135,   134,   135,
     165,     3,     4,     5,     6,     7,     8,     3,     4,     5,
       6,     7,     8,   178,   135,   180,     3,     4,     5,     6,
       7,     8,   135,     3,     4,     5,     6,     7,     8,   134,
     135,   135,   278,   134,   135,   271,   135,   273,   134,   135,
     387,   134,   135,   135,   391,   210,   135,   283,   134,   135,
     286,   134,   135,   134,   135,   135,   186,   293,   188,   295,
     296,   297,   511,   228,   135,   195,   302,   134,   135,   135,
     306,   135,   134,   135,   134,   135,   134,   135,   134,   135,
       3,     4,     5,     6,     7,     8,   134,   135,   135,   134,
     135,   135,   222,   223,   222,   223,   226,   134,   135,   134,
     135,     3,     4,     5,     6,     7,     8,   272,   135,   274,
     134,   135,   134,   135,   279,   135,   352,   135,     3,     4,
       5,     6,     7,     8,     3,     4,     5,     6,     7,     8,
     134,   135,   134,   135,   134,   135,   134,   135,   134,   135,
     134,   135,   307,   134,   135,   135,   276,   134,   135,   135,
     280,   281,   135,   281,   134,   135,   134,   135,   134,   135,
     290,   135,   290,     3,     4,     5,     6,     7,     8,   135,
       3,     4,     5,     6,     7,     8,     3,     4,     5,     6,
       7,     8,     3,     4,     5,     6,     7,     8,     3,     4,
       5,     6,     7,     8,   134,   135,   326,   135,   326,   134,
     135,   135,   438,   134,   135,   135,   442,   134,   135,   445,
     135,   134,   135,     3,     4,     5,     6,     7,     8,   134,
     135,   135,   458,   135,     3,     4,     5,     6,     7,     8,
     135,   396,   134,   135,   399,   400,   472,   134,   135,   404,
     134,   135,   135,   479,   134,   135,   134,   135,   413,   134,
     135,   134,   135,   489,   384,   134,   384,   422,   134,   135,
     134,   135,   392,   393,   134,   135,   135,   432,   398,   135,
     398,     3,     4,     5,     6,     7,     8,   134,   135,   135,
       3,     4,     5,     6,     7,     8,   134,   135,   134,   135,
     134,   135,   134,   135,   134,   135,   134,   135,   134,   135,
     536,   134,   135,   134,   135,   134,   135,   134,   135,   134,
     135,   134,   135,   134,   135,   135,   291,   292,   135,   134,
     135,   135,   452,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   463,   464,   135,   135,   135,    99,   100,
     101,   102,   103,   104,   134,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   134,   135,     3,     4,     5,
       6,     7,     8,   124,   135,   135,   135,   128,   129,   130,
     131,   132,   133,   134,   135,   135,   135,   135,   139,   140,
     119,   511,   143,   511,   135,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   135,   124,     3,     4,     5,     6,
       7,     8,   134,   135,   135,   166,   167,   168,   169,   170,
     171,   134,   173,   174,   175,   176,   177,   547,   179,   132,
     181,   182,   183,   184,   135,   101,   102,   557,   135,   135,
     106,   107,   108,   109,   110,   111,   135,   198,   135,   135,
     135,   202,   203,   204,     3,     4,     5,     6,     7,     8,
     211,   135,   135,   135,   135,   131,   132,   218,   134,   134,
     221,   135,   135,   139,   135,   135,   227,   135,   135,   135,
     146,   147,   148,   135,   150,     3,     4,     5,     6,     7,
       8,     3,     4,     5,     6,     7,     8,   135,   134,   135,
     135,   135,   168,   135,   170,   171,   547,   173,   324,   288,
      -1,    -1,    -1,   179,    -1,   181,   182,   183,   184,    -1,
     271,    -1,   273,   643,   275,   643,    -1,   278,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   287,   134,   135,    -1,
     291,    -1,    -1,   294,   295,    -1,    -1,   298,    -1,    -1,
     301,    -1,    -1,    -1,   305,   306,    -1,    -1,    -1,   310,
     311,   312,    -1,    -1,   315,    -1,    -1,   318,    -1,     3,
       4,     5,     6,     7,     8,     3,     4,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,   134,   135,    -1,    -1,   340,
      -1,    -1,    -1,    -1,   345,    -1,   347,    -1,    -1,   350,
      -1,    -1,   353,    -1,    -1,   356,    -1,   358,     3,     4,
       5,     6,     7,     8,    -1,    -1,   134,   135,    -1,    -1,
      -1,    -1,   134,   135,    -1,   376,    -1,    -1,     3,     4,
       5,     6,     7,     8,    -1,    -1,   387,    -1,    -1,    -1,
     391,    -1,    -1,   394,     3,     4,     5,     6,     7,     8,
      -1,   402,   403,    -1,   405,    -1,    -1,    -1,   409,   410,
     411,   412,    -1,    -1,   415,   416,    -1,    -1,    -1,    -1,
     421,    -1,    -1,   424,    -1,   426,   427,   428,   429,   430,
      -1,    -1,    -1,   434,    -1,    -1,    -1,   438,    -1,    -1,
      -1,   442,    -1,    -1,   445,    -1,    -1,    -1,    -1,   450,
     134,   135,   453,    -1,    -1,    -1,   134,   135,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,   489,   134,
     135,    -1,    -1,    -1,   410,    -1,   412,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   424,   134,
     135,   427,    -1,   429,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   134,   135,    -1,   529,    -1,
      -1,   532,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   553,    -1,   555,    -1,    -1,   558,   559,    -1,
      -1,    -1,    -1,   564,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   574,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   583,    -1,    -1,    -1,    -1,    -1,    -1,   590,
       3,     4,    -1,   594,   134,   135,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,   134,   135,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,     8,     3,     4,     5,     6,
       7,     8,     3,     4,     5,     6,     7,     8,    -1,   122,
       3,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,   134,   135,    -1,    -1,    -1,    -1,   134,   135,    -1,
      -1,    -1,    -1,   134,   135,     3,     4,     5,     6,     7,
       8,   134,   135,     3,     4,     5,     6,     7,     8,    -1,
      -1,   124,     3,     4,     5,     6,     7,     8,    -1,    -1,
      -1,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,   134,   135,    -1,    -1,
      -1,    -1,    -1,    -1,   134,   135,     3,     4,     5,     6,
       7,     8,    -1,   134,   135,     3,     4,     5,     6,     7,
       8,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    -1,    -1,    22,
      -1,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    -1,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    -1,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,     3,     4,     5,     6,     7,     8,   134,   135,     3,
       4,     5,     6,     7,     8,    -1,   134,   135,    26,    -1,
      -1,    -1,   134,   135,     3,     4,     5,     6,     7,     8,
       3,     4,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,   134,    -1,   136,   137,   138,   139,   140,   141,   142,
       3,     4,     5,     6,     7,     8,     3,     4,     5,     6,
       7,     8,     3,     4,     5,     6,     7,     8,     3,     4,
       5,     6,     7,     8,     3,     4,     5,     6,     7,     8,
       3,     4,     5,     6,     7,     8,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,    -1,    -1,    -1,
      -1,    -1,   134,   135,    -1,    -1,    -1,    -1,    -1,    -1,
     134,   135,    -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   134,   135,    -1,    -1,    -1,
      -1,   134,   135,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   134,   135,    -1,    -1,    -1,    -1,   134,   135,    -1,
      -1,    -1,    -1,   134,   135,    -1,    -1,    -1,    -1,   134,
     135,    -1,    -1,    -1,    -1,   134,   135,    -1,    -1,    -1,
      -1,   134,   135,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   134,   144,   145,   150,   151,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,     3,     4,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    22,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,   136,   137,
     138,   139,   140,   141,   142,   150,   151,     0,   151,     3,
       4,     5,     6,     7,     8,   134,   135,   146,   148,   150,
     155,   135,   148,   148,   146,   147,   146,   147,   147,   146,
     146,   148,   147,   147,   147,   147,   147,   147,   146,   158,
     159,   146,   146,   146,   135,   135,   134,   148,   152,   160,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   148,   152,   148,   152,   148,   134,   135,   148,   279,
     151,   146,   135,   134,   152,   152,   146,   152,   146,   146,
     147,   146,   147,   146,   152,   146,   147,   146,   152,   152,
     152,   148,   147,   146,   135,   148,   148,   152,   146,   135,
     148,   148,   146,   147,   147,   146,   147,   146,   146,   147,
     146,   146,   146,   135,   146,   152,     3,   135,   152,   152,
     135,   152,   152,   135,   152,   135,   152,   152,   135,   152,
     148,   146,   146,   147,   146,   147,   147,     3,   156,   147,
     149,   146,   146,   152,   146,   146,   148,   147,   148,   147,
     147,   147,   147,   135,   135,   150,   135,   146,   135,   135,
     146,   135,   135,   135,   135,   135,   148,   135,   135,   135,
     135,   135,   135,   135,   134,   135,   146,   153,   135,   135,
     153,   154,   151,   151,    26,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   152,   135,   152,   148,   152,
     135,   152,   148,   135,   152,   148,   135,   135,   279,   135,
     148,   135,   151,    26,   135,   135,   135,   152,   146,   134,
     135,   276,   135,   151,   134,   135,   281,   134,   135,   281,
     135,   152,   146,   135,   152,   135,   152,   152,   135,   146,
     135,   135,   135,   135,   152,   135,   135,   135,   152,   148,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   134,   135,   289,   135,     3,   135,
     145,   157,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   146,   135,   146,     3,
     135,   135,   146,   135,   135,   152,   146,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   151,   153,   135,
     153,   135,   135,   146,   135,   148,   135,   151,   148,   148,
     135,   135,   146,   146,   148,   146,   135,   135,   135,   146,
     147,   146,   147,   135,   148,   135,   146,   146,   135,   135,
     135,   135,   146,   148,   135,   147,   135,   146,   147,   146,
     146,   147,   146,   135,   148,   135,   146,   135,   135,   135,
     152,   135,   135,   152,   135,   135,   152,   135,   135,   135,
     146,   135,   135,   152,   135,   135,   135,   135,   135,   135,
     152,   135,   135,   135,   135,   152,   135,   135,   135,   152,
     135,   135,     3,   135,   145,   135,   146,   146,   135,   135,
     152,   135,   155,   134,   160,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   134,   135,   135,   135,   135,   135,
     135,   135,   160,   135
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   143,   144,   145,   146,   146,   147,   148,   148,   149,
     149,   150,   150,   151,   151,   151,   152,   152,   152,   153,
     153,   153,   154,   154,   155,   155,   155,   155,   155,   155,
     156,   156,   157,   157,   158,   159,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   183,   183,
     184,   185,   186,   187,   188,   189,   189,   189,   189,   190,
     190,   190,   190,   191,   191,   191,   191,   192,   193,   193,
     193,   193,   193,   194,   195,   196,   197,   198,   199,   199,
     199,   199,   200,   200,   201,   202,   203,   203,   203,   203,
     204,   204,   204,   204,   205,   205,   205,   205,   206,   206,
     206,   206,   207,   208,   209,   210,   210,   211,   212,   212,
     213,   213,   213,   213,   214,   215,   215,   216,   217,   217,
     218,   219,   219,   220,   221,   221,   222,   223,   224,   225,
     225,   225,   226,   227,   227,   227,   227,   228,   228,   229,
     229,   230,   231,   231,   232,   233,   233,   234,   234,   235,
     236,   236,   237,   238,   239,   239,   240,   241,   242,   243,
     243,   244,   245,   246,   247,   248,   248,   248,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   285,   286,
     287,   288,   289,   290,   291
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     2,     2,     1,     3,
       1,     1,     3,     2,     1,     3,     2,     1,     3,     3,
       4,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       3,     2,     2,     1,     2,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     5,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       5,     5,     5,     3,     3,     5,     5,     6,     4,     5,
       5,     6,     4,     5,     5,     6,     4,     4,     5,     5,
       4,     4,     3,     4,     3,     5,     4,     4,     5,     5,
       6,     4,     4,     5,     4,     5,     5,     4,     5,     4,
       5,     5,     6,     4,     5,     4,     5,     4,     5,     5,
       6,     4,     4,     4,     4,     5,     4,     4,     4,     3,
       5,     5,     6,     4,     4,     4,     3,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     3,
       4,     5,     4,     6,     5,     5,     4,     4,     3,     3,
       4,     4,     3,     4,     4,     3,     4,     3,     4,     4,
       3,     4,     4,     4,     4,     3,     4,     4,     4,     4,
       5,     4,     4,     4,     4,     4,     5,     5,     6,     4,
       4,     3,     4,     3,     4,     4,     4,     3,     4,     3,
       4,     4,     4,     3,     3,     3,     4,     4,     4,     4,
       4,     3,     3,     4,     4,     3,     3,     3,     3,     4,
       4,     3,     4,     3,     4,     4,     4,     4,     4,     4,
       3,     4,     3,     4,     6
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, yyscan_t scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, yyscan_t scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, yyscan_t scanner)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 3: /* SYM  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { free(((*yyvaluep).s)); }
#line 2020 "parser.c" /* yacc.c:1258  */
        break;

    case 4: /* STR  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { free(((*yyvaluep).s)); }
#line 2026 "parser.c" /* yacc.c:1258  */
        break;

    case 144: /* start  */
#line 78 "../src/parser.y" /* yacc.c:1258  */
      { run(((*yyvaluep).e));  }
#line 2032 "parser.c" /* yacc.c:1258  */
        break;

    case 145: /* s  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2038 "parser.c" /* yacc.c:1258  */
        break;

    case 146: /* p  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2044 "parser.c" /* yacc.c:1258  */
        break;

    case 147: /* pp  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2050 "parser.c" /* yacc.c:1258  */
        break;

    case 148: /* ps  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2056 "parser.c" /* yacc.c:1258  */
        break;

    case 149: /* pps  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2062 "parser.c" /* yacc.c:1258  */
        break;

    case 150: /* vp  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2068 "parser.c" /* yacc.c:1258  */
        break;

    case 151: /* vps  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2074 "parser.c" /* yacc.c:1258  */
        break;

    case 152: /* opts  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2080 "parser.c" /* yacc.c:1258  */
        break;

    case 153: /* xpb  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2086 "parser.c" /* yacc.c:1258  */
        break;

    case 154: /* xpbs  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2092 "parser.c" /* yacc.c:1258  */
        break;

    case 155: /* np  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2098 "parser.c" /* yacc.c:1258  */
        break;

    case 156: /* sps  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2104 "parser.c" /* yacc.c:1258  */
        break;

    case 157: /* par  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2110 "parser.c" /* yacc.c:1258  */
        break;

    case 158: /* cv  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2116 "parser.c" /* yacc.c:1258  */
        break;

    case 159: /* cvv  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2122 "parser.c" /* yacc.c:1258  */
        break;

    case 160: /* opt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2128 "parser.c" /* yacc.c:1258  */
        break;

    case 161: /* ivp  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2134 "parser.c" /* yacc.c:1258  */
        break;

    case 162: /* add  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2140 "parser.c" /* yacc.c:1258  */
        break;

    case 163: /* div  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2146 "parser.c" /* yacc.c:1258  */
        break;

    case 164: /* mul  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2152 "parser.c" /* yacc.c:1258  */
        break;

    case 165: /* sub  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2158 "parser.c" /* yacc.c:1258  */
        break;

    case 166: /* and  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2164 "parser.c" /* yacc.c:1258  */
        break;

    case 167: /* bitand  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2170 "parser.c" /* yacc.c:1258  */
        break;

    case 168: /* bitnot  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2176 "parser.c" /* yacc.c:1258  */
        break;

    case 169: /* bitor  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2182 "parser.c" /* yacc.c:1258  */
        break;

    case 170: /* bitxor  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2188 "parser.c" /* yacc.c:1258  */
        break;

    case 171: /* not  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2194 "parser.c" /* yacc.c:1258  */
        break;

    case 172: /* in  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2200 "parser.c" /* yacc.c:1258  */
        break;

    case 173: /* or  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2206 "parser.c" /* yacc.c:1258  */
        break;

    case 174: /* shiftleft  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2212 "parser.c" /* yacc.c:1258  */
        break;

    case 175: /* shiftright  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2218 "parser.c" /* yacc.c:1258  */
        break;

    case 176: /* xor  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2224 "parser.c" /* yacc.c:1258  */
        break;

    case 177: /* eq  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2230 "parser.c" /* yacc.c:1258  */
        break;

    case 178: /* gt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2236 "parser.c" /* yacc.c:1258  */
        break;

    case 179: /* gte  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2242 "parser.c" /* yacc.c:1258  */
        break;

    case 180: /* lt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2248 "parser.c" /* yacc.c:1258  */
        break;

    case 181: /* lte  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2254 "parser.c" /* yacc.c:1258  */
        break;

    case 182: /* neq  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2260 "parser.c" /* yacc.c:1258  */
        break;

    case 183: /* if  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2266 "parser.c" /* yacc.c:1258  */
        break;

    case 184: /* select  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2272 "parser.c" /* yacc.c:1258  */
        break;

    case 185: /* until  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2278 "parser.c" /* yacc.c:1258  */
        break;

    case 186: /* while  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2284 "parser.c" /* yacc.c:1258  */
        break;

    case 187: /* trace  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2290 "parser.c" /* yacc.c:1258  */
        break;

    case 188: /* retrace  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2296 "parser.c" /* yacc.c:1258  */
        break;

    case 189: /* execute  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2302 "parser.c" /* yacc.c:1258  */
        break;

    case 190: /* rexx  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2308 "parser.c" /* yacc.c:1258  */
        break;

    case 191: /* run  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2314 "parser.c" /* yacc.c:1258  */
        break;

    case 192: /* abort  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2320 "parser.c" /* yacc.c:1258  */
        break;

    case 193: /* exit  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2326 "parser.c" /* yacc.c:1258  */
        break;

    case 194: /* onerror  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2332 "parser.c" /* yacc.c:1258  */
        break;

    case 195: /* reboot  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2338 "parser.c" /* yacc.c:1258  */
        break;

    case 196: /* trap  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2344 "parser.c" /* yacc.c:1258  */
        break;

    case 197: /* copyfiles  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2350 "parser.c" /* yacc.c:1258  */
        break;

    case 198: /* copylib  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2356 "parser.c" /* yacc.c:1258  */
        break;

    case 199: /* delete  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2362 "parser.c" /* yacc.c:1258  */
        break;

    case 200: /* exists  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2368 "parser.c" /* yacc.c:1258  */
        break;

    case 201: /* fileonly  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2374 "parser.c" /* yacc.c:1258  */
        break;

    case 202: /* foreach  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2380 "parser.c" /* yacc.c:1258  */
        break;

    case 203: /* makeassign  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2386 "parser.c" /* yacc.c:1258  */
        break;

    case 204: /* makedir  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2392 "parser.c" /* yacc.c:1258  */
        break;

    case 205: /* protect  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2398 "parser.c" /* yacc.c:1258  */
        break;

    case 206: /* startup  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2404 "parser.c" /* yacc.c:1258  */
        break;

    case 207: /* textfile  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2410 "parser.c" /* yacc.c:1258  */
        break;

    case 208: /* tooltype  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2416 "parser.c" /* yacc.c:1258  */
        break;

    case 209: /* transcript  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2422 "parser.c" /* yacc.c:1258  */
        break;

    case 210: /* rename  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2428 "parser.c" /* yacc.c:1258  */
        break;

    case 211: /* complete  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2434 "parser.c" /* yacc.c:1258  */
        break;

    case 212: /* debug  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2440 "parser.c" /* yacc.c:1258  */
        break;

    case 213: /* message  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2446 "parser.c" /* yacc.c:1258  */
        break;

    case 214: /* user  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2452 "parser.c" /* yacc.c:1258  */
        break;

    case 215: /* welcome  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2458 "parser.c" /* yacc.c:1258  */
        break;

    case 216: /* working  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2464 "parser.c" /* yacc.c:1258  */
        break;

    case 217: /* database  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2470 "parser.c" /* yacc.c:1258  */
        break;

    case 218: /* earlier  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2476 "parser.c" /* yacc.c:1258  */
        break;

    case 219: /* getassign  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2482 "parser.c" /* yacc.c:1258  */
        break;

    case 220: /* getdevice  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2488 "parser.c" /* yacc.c:1258  */
        break;

    case 221: /* getdiskspace  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2494 "parser.c" /* yacc.c:1258  */
        break;

    case 222: /* getenv  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2500 "parser.c" /* yacc.c:1258  */
        break;

    case 223: /* getsize  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2506 "parser.c" /* yacc.c:1258  */
        break;

    case 224: /* getsum  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2512 "parser.c" /* yacc.c:1258  */
        break;

    case 225: /* getversion  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2518 "parser.c" /* yacc.c:1258  */
        break;

    case 226: /* iconinfo  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2524 "parser.c" /* yacc.c:1258  */
        break;

    case 227: /* dcl  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2530 "parser.c" /* yacc.c:1258  */
        break;

    case 228: /* cus  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2536 "parser.c" /* yacc.c:1258  */
        break;

    case 229: /* askbool  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2542 "parser.c" /* yacc.c:1258  */
        break;

    case 230: /* askchoice  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2548 "parser.c" /* yacc.c:1258  */
        break;

    case 231: /* askdir  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2554 "parser.c" /* yacc.c:1258  */
        break;

    case 232: /* askdisk  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2560 "parser.c" /* yacc.c:1258  */
        break;

    case 233: /* askfile  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2566 "parser.c" /* yacc.c:1258  */
        break;

    case 234: /* asknumber  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2572 "parser.c" /* yacc.c:1258  */
        break;

    case 235: /* askoptions  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2578 "parser.c" /* yacc.c:1258  */
        break;

    case 236: /* askstring  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2584 "parser.c" /* yacc.c:1258  */
        break;

    case 237: /* cat  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2590 "parser.c" /* yacc.c:1258  */
        break;

    case 238: /* expandpath  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2596 "parser.c" /* yacc.c:1258  */
        break;

    case 239: /* fmt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2602 "parser.c" /* yacc.c:1258  */
        break;

    case 240: /* pathonly  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2608 "parser.c" /* yacc.c:1258  */
        break;

    case 241: /* patmatch  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2614 "parser.c" /* yacc.c:1258  */
        break;

    case 242: /* strlen  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2620 "parser.c" /* yacc.c:1258  */
        break;

    case 243: /* substr  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2626 "parser.c" /* yacc.c:1258  */
        break;

    case 244: /* tackon  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2632 "parser.c" /* yacc.c:1258  */
        break;

    case 245: /* set  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2638 "parser.c" /* yacc.c:1258  */
        break;

    case 246: /* symbolset  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2644 "parser.c" /* yacc.c:1258  */
        break;

    case 247: /* symbolval  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2650 "parser.c" /* yacc.c:1258  */
        break;

    case 248: /* openwbobject  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2656 "parser.c" /* yacc.c:1258  */
        break;

    case 249: /* showwbobject  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2662 "parser.c" /* yacc.c:1258  */
        break;

    case 250: /* closewbobject  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2668 "parser.c" /* yacc.c:1258  */
        break;

    case 251: /* all  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2674 "parser.c" /* yacc.c:1258  */
        break;

    case 252: /* append  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2680 "parser.c" /* yacc.c:1258  */
        break;

    case 253: /* assigns  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2686 "parser.c" /* yacc.c:1258  */
        break;

    case 254: /* back  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2692 "parser.c" /* yacc.c:1258  */
        break;

    case 255: /* choices  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2698 "parser.c" /* yacc.c:1258  */
        break;

    case 256: /* command  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2704 "parser.c" /* yacc.c:1258  */
        break;

    case 257: /* compression  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2710 "parser.c" /* yacc.c:1258  */
        break;

    case 258: /* confirm  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2716 "parser.c" /* yacc.c:1258  */
        break;

    case 259: /* default  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2722 "parser.c" /* yacc.c:1258  */
        break;

    case 260: /* delopts  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2728 "parser.c" /* yacc.c:1258  */
        break;

    case 261: /* dest  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2734 "parser.c" /* yacc.c:1258  */
        break;

    case 262: /* disk  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2740 "parser.c" /* yacc.c:1258  */
        break;

    case 263: /* files  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2746 "parser.c" /* yacc.c:1258  */
        break;

    case 264: /* fonts  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2752 "parser.c" /* yacc.c:1258  */
        break;

    case 265: /* getdefaulttool  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2758 "parser.c" /* yacc.c:1258  */
        break;

    case 266: /* getposition  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2764 "parser.c" /* yacc.c:1258  */
        break;

    case 267: /* getstack  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2770 "parser.c" /* yacc.c:1258  */
        break;

    case 268: /* gettooltype  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2776 "parser.c" /* yacc.c:1258  */
        break;

    case 269: /* help  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2782 "parser.c" /* yacc.c:1258  */
        break;

    case 270: /* infos  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2788 "parser.c" /* yacc.c:1258  */
        break;

    case 271: /* include  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2794 "parser.c" /* yacc.c:1258  */
        break;

    case 272: /* newname  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2800 "parser.c" /* yacc.c:1258  */
        break;

    case 273: /* newpath  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2806 "parser.c" /* yacc.c:1258  */
        break;

    case 274: /* nogauge  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2812 "parser.c" /* yacc.c:1258  */
        break;

    case 275: /* noposition  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2818 "parser.c" /* yacc.c:1258  */
        break;

    case 276: /* noreq  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2824 "parser.c" /* yacc.c:1258  */
        break;

    case 277: /* pattern  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2830 "parser.c" /* yacc.c:1258  */
        break;

    case 278: /* prompt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2836 "parser.c" /* yacc.c:1258  */
        break;

    case 279: /* quiet  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2842 "parser.c" /* yacc.c:1258  */
        break;

    case 280: /* range  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2848 "parser.c" /* yacc.c:1258  */
        break;

    case 281: /* safe  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2854 "parser.c" /* yacc.c:1258  */
        break;

    case 282: /* setdefaulttool  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2860 "parser.c" /* yacc.c:1258  */
        break;

    case 283: /* setposition  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2866 "parser.c" /* yacc.c:1258  */
        break;

    case 284: /* setstack  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2872 "parser.c" /* yacc.c:1258  */
        break;

    case 285: /* settooltype  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2878 "parser.c" /* yacc.c:1258  */
        break;

    case 286: /* source  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2884 "parser.c" /* yacc.c:1258  */
        break;

    case 287: /* swapcolors  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2890 "parser.c" /* yacc.c:1258  */
        break;

    case 288: /* optional  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2896 "parser.c" /* yacc.c:1258  */
        break;

    case 289: /* resident  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2902 "parser.c" /* yacc.c:1258  */
        break;

    case 290: /* override  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2908 "parser.c" /* yacc.c:1258  */
        break;

    case 291: /* dynopt  */
#line 83 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2914 "parser.c" /* yacc.c:1258  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (yyscan_t scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 99 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = init((yyvsp[0].e)); }
#line 3182 "parser.c" /* yacc.c:1651  */
    break;

  case 6:
#line 105 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3188 "parser.c" /* yacc.c:1651  */
    break;

  case 7:
#line 106 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3194 "parser.c" /* yacc.c:1651  */
    break;

  case 8:
#line 107 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3200 "parser.c" /* yacc.c:1651  */
    break;

  case 9:
#line 108 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push((yyvsp[-2].e), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3206 "parser.c" /* yacc.c:1651  */
    break;

  case 12:
#line 111 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3212 "parser.c" /* yacc.c:1651  */
    break;

  case 13:
#line 112 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = merge((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3218 "parser.c" /* yacc.c:1651  */
    break;

  case 14:
#line 113 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3224 "parser.c" /* yacc.c:1651  */
    break;

  case 15:
#line 114 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3230 "parser.c" /* yacc.c:1651  */
    break;

  case 16:
#line 115 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = merge((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3236 "parser.c" /* yacc.c:1651  */
    break;

  case 17:
#line 116 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3242 "parser.c" /* yacc.c:1651  */
    break;

  case 18:
#line 117 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3248 "parser.c" /* yacc.c:1651  */
    break;

  case 19:
#line 118 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3254 "parser.c" /* yacc.c:1651  */
    break;

  case 20:
#line 119 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-2].e), (yyvsp[-1].e)); }
#line 3260 "parser.c" /* yacc.c:1651  */
    break;

  case 21:
#line 120 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3266 "parser.c" /* yacc.c:1651  */
    break;

  case 22:
#line 121 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3272 "parser.c" /* yacc.c:1651  */
    break;

  case 23:
#line 122 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3278 "parser.c" /* yacc.c:1651  */
    break;

  case 24:
#line 123 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3284 "parser.c" /* yacc.c:1651  */
    break;

  case 25:
#line 124 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3290 "parser.c" /* yacc.c:1651  */
    break;

  case 26:
#line 125 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3296 "parser.c" /* yacc.c:1651  */
    break;

  case 27:
#line 126 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_string((yyvsp[0].s)); }
#line 3302 "parser.c" /* yacc.c:1651  */
    break;

  case 28:
#line 127 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_symref((yyvsp[0].s), LINE); }
#line 3308 "parser.c" /* yacc.c:1651  */
    break;

  case 29:
#line 128 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = NULL; YYFPRINTF(stderr, "Out of memory in line %d\n", LINE); YYABORT; }
#line 3314 "parser.c" /* yacc.c:1651  */
    break;

  case 30:
#line 129 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push((yyvsp[-2].e), new_symbol((yyvsp[-1].s))), (yyvsp[0].e)) ; }
#line 3320 "parser.c" /* yacc.c:1651  */
    break;

  case 31:
#line 130 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), new_symbol((yyvsp[-1].s))), (yyvsp[0].e)); }
#line 3326 "parser.c" /* yacc.c:1651  */
    break;

  case 32:
#line 131 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), new_symbol((yyvsp[0].s))); }
#line 3332 "parser.c" /* yacc.c:1651  */
    break;

  case 33:
#line 132 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), new_symbol((yyvsp[0].s))); }
#line 3338 "parser.c" /* yacc.c:1651  */
    break;

  case 34:
#line 133 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3344 "parser.c" /* yacc.c:1651  */
    break;

  case 35:
#line 134 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3350 "parser.c" /* yacc.c:1651  */
    break;

  case 166:
#line 271 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("+"), LINE, m_add, (yyvsp[-1].e), NUMBER); }
#line 3356 "parser.c" /* yacc.c:1651  */
    break;

  case 167:
#line 272 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("/"), LINE, m_div, (yyvsp[-1].e), NUMBER); }
#line 3362 "parser.c" /* yacc.c:1651  */
    break;

  case 168:
#line 273 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("*"), LINE, m_mul, (yyvsp[-1].e), NUMBER); }
#line 3368 "parser.c" /* yacc.c:1651  */
    break;

  case 169:
#line 274 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("-"), LINE, m_sub, (yyvsp[-1].e), NUMBER); }
#line 3374 "parser.c" /* yacc.c:1651  */
    break;

  case 170:
#line 277 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("AND"), LINE, m_and, (yyvsp[-1].e), NUMBER); }
#line 3380 "parser.c" /* yacc.c:1651  */
    break;

  case 171:
#line 278 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITAND"), LINE, m_bitand, (yyvsp[-1].e), NUMBER); }
#line 3386 "parser.c" /* yacc.c:1651  */
    break;

  case 172:
#line 279 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITNOT"), LINE, m_bitnot, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3392 "parser.c" /* yacc.c:1651  */
    break;

  case 173:
#line 280 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITOR"), LINE, m_bitor, (yyvsp[-1].e), NUMBER); }
#line 3398 "parser.c" /* yacc.c:1651  */
    break;

  case 174:
#line 281 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITXOR"), LINE, m_bitxor, (yyvsp[-1].e), NUMBER); }
#line 3404 "parser.c" /* yacc.c:1651  */
    break;

  case 175:
#line 282 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("NOT"), LINE, m_not, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3410 "parser.c" /* yacc.c:1651  */
    break;

  case 176:
#line 283 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("IN"), LINE, m_in, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3416 "parser.c" /* yacc.c:1651  */
    break;

  case 177:
#line 284 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("OR"), LINE, m_or, (yyvsp[-1].e), NUMBER); }
#line 3422 "parser.c" /* yacc.c:1651  */
    break;

  case 178:
#line 285 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("shiftleft"), LINE, m_shiftleft, (yyvsp[-1].e), NUMBER); }
#line 3428 "parser.c" /* yacc.c:1651  */
    break;

  case 179:
#line 286 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("shiftright"), LINE, m_shiftright, (yyvsp[-1].e), NUMBER); }
#line 3434 "parser.c" /* yacc.c:1651  */
    break;

  case 180:
#line 287 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("XOR"), LINE, m_xor, (yyvsp[-1].e), NUMBER); }
#line 3440 "parser.c" /* yacc.c:1651  */
    break;

  case 181:
#line 290 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("="), LINE, m_eq, (yyvsp[-1].e), NUMBER); }
#line 3446 "parser.c" /* yacc.c:1651  */
    break;

  case 182:
#line 291 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup(">"), LINE, m_gt, (yyvsp[-1].e), NUMBER); }
#line 3452 "parser.c" /* yacc.c:1651  */
    break;

  case 183:
#line 292 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup(">="), LINE, m_gte, (yyvsp[-1].e), NUMBER); }
#line 3458 "parser.c" /* yacc.c:1651  */
    break;

  case 184:
#line 293 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<"), LINE, m_lt, (yyvsp[-1].e), NUMBER); }
#line 3464 "parser.c" /* yacc.c:1651  */
    break;

  case 185:
#line 294 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<="), LINE, m_lte, (yyvsp[-1].e), NUMBER); }
#line 3470 "parser.c" /* yacc.c:1651  */
    break;

  case 186:
#line 295 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<>"), LINE, m_neq, (yyvsp[-1].e), NUMBER); }
#line 3476 "parser.c" /* yacc.c:1651  */
    break;

  case 187:
#line 298 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, (yyvsp[-1].e), NUMBER); }
#line 3482 "parser.c" /* yacc.c:1651  */
    break;

  case 188:
#line 299 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, (yyvsp[-1].e), NUMBER); }
#line 3488 "parser.c" /* yacc.c:1651  */
    break;

  case 189:
#line 300 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3494 "parser.c" /* yacc.c:1651  */
    break;

  case 190:
#line 301 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("select"), LINE, m_select, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3500 "parser.c" /* yacc.c:1651  */
    break;

  case 191:
#line 302 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("until"), LINE, m_until, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3506 "parser.c" /* yacc.c:1651  */
    break;

  case 192:
#line 303 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("while"), LINE, m_while, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3512 "parser.c" /* yacc.c:1651  */
    break;

  case 193:
#line 304 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("trace"), LINE, m_trace, NULL, NUMBER); }
#line 3518 "parser.c" /* yacc.c:1651  */
    break;

  case 194:
#line 305 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("retrace"), LINE, m_retrace, NULL, DANGLE); }
#line 3524 "parser.c" /* yacc.c:1651  */
    break;

  case 195:
#line 308 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3530 "parser.c" /* yacc.c:1651  */
    break;

  case 196:
#line 309 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3536 "parser.c" /* yacc.c:1651  */
    break;

  case 197:
#line 310 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3542 "parser.c" /* yacc.c:1651  */
    break;

  case 198:
#line 311 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, (yyvsp[-1].e), NUMBER); }
#line 3548 "parser.c" /* yacc.c:1651  */
    break;

  case 199:
#line 312 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3554 "parser.c" /* yacc.c:1651  */
    break;

  case 200:
#line 313 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3560 "parser.c" /* yacc.c:1651  */
    break;

  case 201:
#line 314 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3566 "parser.c" /* yacc.c:1651  */
    break;

  case 202:
#line 315 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, (yyvsp[-1].e), NUMBER); }
#line 3572 "parser.c" /* yacc.c:1651  */
    break;

  case 203:
#line 316 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3578 "parser.c" /* yacc.c:1651  */
    break;

  case 204:
#line 317 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3584 "parser.c" /* yacc.c:1651  */
    break;

  case 205:
#line 318 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3590 "parser.c" /* yacc.c:1651  */
    break;

  case 206:
#line 319 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, (yyvsp[-1].e), NUMBER); }
#line 3596 "parser.c" /* yacc.c:1651  */
    break;

  case 207:
#line 322 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("abort"), LINE, m_abort, (yyvsp[-1].e), NUMBER); }
#line 3602 "parser.c" /* yacc.c:1651  */
    break;

  case 208:
#line 323 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3608 "parser.c" /* yacc.c:1651  */
    break;

  case 209:
#line 324 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3614 "parser.c" /* yacc.c:1651  */
    break;

  case 210:
#line 325 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3620 "parser.c" /* yacc.c:1651  */
    break;

  case 211:
#line 326 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, (yyvsp[-1].e), NUMBER); }
#line 3626 "parser.c" /* yacc.c:1651  */
    break;

  case 212:
#line 327 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, NULL, NUMBER); }
#line 3632 "parser.c" /* yacc.c:1651  */
    break;

  case 213:
#line 328 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("onerror"), LINE, m_procedure, push(new_contxt(),
                                                        new_custom(strdup("@onerror"), LINE, NULL, (yyvsp[-1].e))), DANGLE); }
#line 3639 "parser.c" /* yacc.c:1651  */
    break;

  case 214:
#line 330 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("reboot"), LINE, m_reboot, NULL, NUMBER); }
#line 3645 "parser.c" /* yacc.c:1651  */
    break;

  case 215:
#line 331 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("trap"), LINE, m_trap, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3651 "parser.c" /* yacc.c:1651  */
    break;

  case 216:
#line 334 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("copyfiles"), LINE, m_copyfiles, (yyvsp[-1].e), NUMBER); }
#line 3657 "parser.c" /* yacc.c:1651  */
    break;

  case 217:
#line 335 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("copylib"), LINE, m_copylib, (yyvsp[-1].e), NUMBER); }
#line 3663 "parser.c" /* yacc.c:1651  */
    break;

  case 218:
#line 336 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3669 "parser.c" /* yacc.c:1651  */
    break;

  case 219:
#line 337 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3675 "parser.c" /* yacc.c:1651  */
    break;

  case 220:
#line 338 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3681 "parser.c" /* yacc.c:1651  */
    break;

  case 221:
#line 339 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3687 "parser.c" /* yacc.c:1651  */
    break;

  case 222:
#line 340 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exists"), LINE, m_exists, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3693 "parser.c" /* yacc.c:1651  */
    break;

  case 223:
#line 341 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exists"), LINE, m_exists, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3699 "parser.c" /* yacc.c:1651  */
    break;

  case 224:
#line 342 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("fileonly"), LINE, m_fileonly, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3705 "parser.c" /* yacc.c:1651  */
    break;

  case 225:
#line 343 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("foreach"), LINE, m_foreach, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3711 "parser.c" /* yacc.c:1651  */
    break;

  case 226:
#line 344 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3717 "parser.c" /* yacc.c:1651  */
    break;

  case 227:
#line 345 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, (yyvsp[-1].e), NUMBER); }
#line 3723 "parser.c" /* yacc.c:1651  */
    break;

  case 228:
#line 346 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3729 "parser.c" /* yacc.c:1651  */
    break;

  case 229:
#line 347 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3735 "parser.c" /* yacc.c:1651  */
    break;

  case 230:
#line 348 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3741 "parser.c" /* yacc.c:1651  */
    break;

  case 231:
#line 349 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3747 "parser.c" /* yacc.c:1651  */
    break;

  case 232:
#line 350 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3753 "parser.c" /* yacc.c:1651  */
    break;

  case 233:
#line 351 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3759 "parser.c" /* yacc.c:1651  */
    break;

  case 234:
#line 352 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3765 "parser.c" /* yacc.c:1651  */
    break;

  case 235:
#line 353 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, (yyvsp[-1].e), NUMBER); }
#line 3771 "parser.c" /* yacc.c:1651  */
    break;

  case 236:
#line 354 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3777 "parser.c" /* yacc.c:1651  */
    break;

  case 237:
#line 355 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3783 "parser.c" /* yacc.c:1651  */
    break;

  case 238:
#line 356 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3789 "parser.c" /* yacc.c:1651  */
    break;

  case 239:
#line 357 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3795 "parser.c" /* yacc.c:1651  */
    break;

  case 240:
#line 358 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3801 "parser.c" /* yacc.c:1651  */
    break;

  case 241:
#line 359 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), new_symref(strdup("@app-name"), LINE)), (yyvsp[-1].e)), NUMBER); }
#line 3807 "parser.c" /* yacc.c:1651  */
    break;

  case 242:
#line 360 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("textfile"), LINE, m_textfile, (yyvsp[-1].e), NUMBER); }
#line 3813 "parser.c" /* yacc.c:1651  */
    break;

  case 243:
#line 361 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("tooltype"), LINE, m_tooltype, (yyvsp[-1].e), NUMBER); }
#line 3819 "parser.c" /* yacc.c:1651  */
    break;

  case 244:
#line 362 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("transcript"), LINE, m_transcript, (yyvsp[-1].e), NUMBER); }
#line 3825 "parser.c" /* yacc.c:1651  */
    break;

  case 245:
#line 363 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rename"), LINE, m_rename, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3831 "parser.c" /* yacc.c:1651  */
    break;

  case 246:
#line 364 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rename"), LINE, m_rename, (yyvsp[-1].e), NUMBER); }
#line 3837 "parser.c" /* yacc.c:1651  */
    break;

  case 247:
#line 367 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("complete"), LINE, m_complete, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3843 "parser.c" /* yacc.c:1651  */
    break;

  case 248:
#line 368 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("debug"), LINE, m_debug, (yyvsp[-1].e), NUMBER); }
#line 3849 "parser.c" /* yacc.c:1651  */
    break;

  case 249:
#line 369 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("debug"), LINE, m_debug, NULL, NUMBER); }
#line 3855 "parser.c" /* yacc.c:1651  */
    break;

  case 250:
#line 370 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, merge((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3861 "parser.c" /* yacc.c:1651  */
    break;

  case 251:
#line 371 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, merge((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3867 "parser.c" /* yacc.c:1651  */
    break;

  case 252:
#line 372 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, push(merge((yyvsp[-3].e), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3873 "parser.c" /* yacc.c:1651  */
    break;

  case 253:
#line 373 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, (yyvsp[-1].e), NUMBER); }
#line 3879 "parser.c" /* yacc.c:1651  */
    break;

  case 254:
#line 374 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("user"), LINE, m_user, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3885 "parser.c" /* yacc.c:1651  */
    break;

  case 255:
#line 375 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("welcome"), LINE, m_welcome, (yyvsp[-1].e), NUMBER); }
#line 3891 "parser.c" /* yacc.c:1651  */
    break;

  case 256:
#line 376 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("welcome"), LINE, m_welcome, NULL, NUMBER); }
#line 3897 "parser.c" /* yacc.c:1651  */
    break;

  case 257:
#line 377 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("working"), LINE, m_working, (yyvsp[-1].e), NUMBER); }
#line 3903 "parser.c" /* yacc.c:1651  */
    break;

  case 258:
#line 380 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("database"), LINE, m_database, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3909 "parser.c" /* yacc.c:1651  */
    break;

  case 259:
#line 381 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("database"), LINE, m_database, (yyvsp[-1].e), STRING); }
#line 3915 "parser.c" /* yacc.c:1651  */
    break;

  case 260:
#line 382 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("earlier"), LINE, m_earlier, (yyvsp[-1].e), NUMBER); }
#line 3921 "parser.c" /* yacc.c:1651  */
    break;

  case 261:
#line 383 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getassign"), LINE, m_getassign, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3927 "parser.c" /* yacc.c:1651  */
    break;

  case 262:
#line 384 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getassign"), LINE, m_getassign, (yyvsp[-1].e), STRING); }
#line 3933 "parser.c" /* yacc.c:1651  */
    break;

  case 263:
#line 385 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdevice"), LINE, m_getdevice, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3939 "parser.c" /* yacc.c:1651  */
    break;

  case 264:
#line 386 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdiskspace"), LINE, m_getdiskspace, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3945 "parser.c" /* yacc.c:1651  */
    break;

  case 265:
#line 387 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdiskspace"), LINE, m_getdiskspace, (yyvsp[-1].e), NUMBER); }
#line 3951 "parser.c" /* yacc.c:1651  */
    break;

  case 266:
#line 388 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getenv"), LINE, m_getenv, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3957 "parser.c" /* yacc.c:1651  */
    break;

  case 267:
#line 389 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getsize"), LINE, m_getsize, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3963 "parser.c" /* yacc.c:1651  */
    break;

  case 268:
#line 390 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getsum"), LINE, m_getsum, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3969 "parser.c" /* yacc.c:1651  */
    break;

  case 269:
#line 391 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, NULL, NUMBER); }
#line 3975 "parser.c" /* yacc.c:1651  */
    break;

  case 270:
#line 392 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3981 "parser.c" /* yacc.c:1651  */
    break;

  case 271:
#line 393 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3987 "parser.c" /* yacc.c:1651  */
    break;

  case 272:
#line 394 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("iconinfo"), LINE, m_iconinfo, (yyvsp[-1].e), NUMBER); }
#line 3993 "parser.c" /* yacc.c:1651  */
    break;

  case 273:
#line 397 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-3].s), LINE, (yyvsp[-2].e), (yyvsp[-1].e))), NUMBER); }
#line 3999 "parser.c" /* yacc.c:1651  */
    break;

  case 274:
#line 398 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-2].s), LINE, (yyvsp[-1].e), NULL)), NUMBER); }
#line 4005 "parser.c" /* yacc.c:1651  */
    break;

  case 275:
#line 399 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-2].s), LINE, NULL, (yyvsp[-1].e))), NUMBER); }
#line 4011 "parser.c" /* yacc.c:1651  */
    break;

  case 276:
#line 400 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-1].s), LINE, NULL, NULL)), NUMBER); }
#line 4017 "parser.c" /* yacc.c:1651  */
    break;

  case 277:
#line 401 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_cusref((yyvsp[-2].s), LINE, (yyvsp[-1].e)); }
#line 4023 "parser.c" /* yacc.c:1651  */
    break;

  case 278:
#line 402 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_cusref((yyvsp[-1].s), LINE, NULL); }
#line 4029 "parser.c" /* yacc.c:1651  */
    break;

  case 279:
#line 405 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askbool"), LINE, m_askbool, NULL, NUMBER); }
#line 4035 "parser.c" /* yacc.c:1651  */
    break;

  case 280:
#line 406 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askbool"), LINE, m_askbool, (yyvsp[-1].e), NUMBER); }
#line 4041 "parser.c" /* yacc.c:1651  */
    break;

  case 281:
#line 407 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askchoice"), LINE, m_askchoice, (yyvsp[-1].e), NUMBER); }
#line 4047 "parser.c" /* yacc.c:1651  */
    break;

  case 282:
#line 408 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdir"), LINE, m_askdir, NULL, STRING); }
#line 4053 "parser.c" /* yacc.c:1651  */
    break;

  case 283:
#line 409 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdir"), LINE, m_askdir, (yyvsp[-1].e), STRING); }
#line 4059 "parser.c" /* yacc.c:1651  */
    break;

  case 284:
#line 410 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdisk"), LINE, m_askdisk, (yyvsp[-1].e), NUMBER); }
#line 4065 "parser.c" /* yacc.c:1651  */
    break;

  case 285:
#line 411 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askfile"), LINE, m_askfile, NULL, STRING); }
#line 4071 "parser.c" /* yacc.c:1651  */
    break;

  case 286:
#line 412 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askfile"), LINE, m_askfile, (yyvsp[-1].e), STRING); }
#line 4077 "parser.c" /* yacc.c:1651  */
    break;

  case 287:
#line 413 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("asknumber"), LINE, m_asknumber, NULL, NUMBER); }
#line 4083 "parser.c" /* yacc.c:1651  */
    break;

  case 288:
#line 414 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("asknumber"), LINE, m_asknumber, (yyvsp[-1].e), NUMBER); }
#line 4089 "parser.c" /* yacc.c:1651  */
    break;

  case 289:
#line 415 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askoptions"), LINE, m_askoptions, (yyvsp[-1].e), NUMBER); }
#line 4095 "parser.c" /* yacc.c:1651  */
    break;

  case 290:
#line 416 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askstring"), LINE, m_askstring, NULL, STRING); }
#line 4101 "parser.c" /* yacc.c:1651  */
    break;

  case 291:
#line 417 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askstring"), LINE, m_askstring, (yyvsp[-1].e), STRING); }
#line 4107 "parser.c" /* yacc.c:1651  */
    break;

  case 292:
#line 420 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("cat"), LINE, m_cat, (yyvsp[-1].e), STRING); }
#line 4113 "parser.c" /* yacc.c:1651  */
    break;

  case 293:
#line 421 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("expandpath"), LINE, m_expandpath, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 4119 "parser.c" /* yacc.c:1651  */
    break;

  case 294:
#line 422 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native((yyvsp[-2].s), LINE, m_fmt, (yyvsp[-1].e), STRING); }
#line 4125 "parser.c" /* yacc.c:1651  */
    break;

  case 295:
#line 423 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native((yyvsp[-1].s), LINE, m_fmt, NULL, STRING); }
#line 4131 "parser.c" /* yacc.c:1651  */
    break;

  case 296:
#line 424 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("pathonly"), LINE, m_pathonly, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 4137 "parser.c" /* yacc.c:1651  */
    break;

  case 297:
#line 425 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("patmatch"), LINE, m_patmatch, (yyvsp[-1].e), NUMBER); }
#line 4143 "parser.c" /* yacc.c:1651  */
    break;

  case 298:
#line 426 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("strlen"), LINE, m_strlen, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4149 "parser.c" /* yacc.c:1651  */
    break;

  case 299:
#line 427 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("substr"), LINE, m_substr, (yyvsp[-1].e), STRING); }
#line 4155 "parser.c" /* yacc.c:1651  */
    break;

  case 300:
#line 428 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("substr"), LINE, m_substr, push((yyvsp[-2].e), (yyvsp[-1].e)), STRING); }
#line 4161 "parser.c" /* yacc.c:1651  */
    break;

  case 301:
#line 429 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("tackon"), LINE, m_tackon, (yyvsp[-1].e), STRING); }
#line 4167 "parser.c" /* yacc.c:1651  */
    break;

  case 302:
#line 432 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("set"), LINE, m_set, (yyvsp[-1].e), DANGLE); }
#line 4173 "parser.c" /* yacc.c:1651  */
    break;

  case 303:
#line 433 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("symbolset"), LINE, m_symbolset, (yyvsp[-1].e), DANGLE); }
#line 4179 "parser.c" /* yacc.c:1651  */
    break;

  case 304:
#line 434 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("symbolval"), LINE, m_symbolval, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4185 "parser.c" /* yacc.c:1651  */
    break;

  case 305:
#line 437 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4191 "parser.c" /* yacc.c:1651  */
    break;

  case 306:
#line 438 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 4197 "parser.c" /* yacc.c:1651  */
    break;

  case 307:
#line 439 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 4203 "parser.c" /* yacc.c:1651  */
    break;

  case 308:
#line 440 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 4209 "parser.c" /* yacc.c:1651  */
    break;

  case 309:
#line 441 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("showwbobject"), LINE, m_showwbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4215 "parser.c" /* yacc.c:1651  */
    break;

  case 310:
#line 442 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("closewbobject"), LINE, m_closewbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4221 "parser.c" /* yacc.c:1651  */
    break;

  case 311:
#line 445 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("all"), OPT_ALL, NULL); }
#line 4227 "parser.c" /* yacc.c:1651  */
    break;

  case 312:
#line 446 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("append"), OPT_APPEND, (yyvsp[-1].e)); }
#line 4233 "parser.c" /* yacc.c:1651  */
    break;

  case 313:
#line 447 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("assigns"), OPT_ASSIGNS, NULL); }
#line 4239 "parser.c" /* yacc.c:1651  */
    break;

  case 314:
#line 448 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("back"), OPT_BACK, (yyvsp[-1].e)); }
#line 4245 "parser.c" /* yacc.c:1651  */
    break;

  case 315:
#line 449 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("choices"), OPT_CHOICES, (yyvsp[-1].e)); }
#line 4251 "parser.c" /* yacc.c:1651  */
    break;

  case 316:
#line 450 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("command"), OPT_COMMAND, (yyvsp[-1].e)); }
#line 4257 "parser.c" /* yacc.c:1651  */
    break;

  case 317:
#line 451 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("compression"), OPT_COMPRESSION, NULL); }
#line 4263 "parser.c" /* yacc.c:1651  */
    break;

  case 318:
#line 452 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("confirm"), OPT_CONFIRM, push(new_contxt(), (yyvsp[-1].e))); }
#line 4269 "parser.c" /* yacc.c:1651  */
    break;

  case 319:
#line 453 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("confirm"), OPT_CONFIRM, NULL); }
#line 4275 "parser.c" /* yacc.c:1651  */
    break;

  case 320:
#line 454 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("default"), OPT_DEFAULT, push(new_contxt(), (yyvsp[-1].e))); }
#line 4281 "parser.c" /* yacc.c:1651  */
    break;

  case 321:
#line 455 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("delopts"), OPT_DELOPTS, (yyvsp[-1].e)); }
#line 4287 "parser.c" /* yacc.c:1651  */
    break;

  case 322:
#line 456 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("dest"), OPT_DEST, push(new_contxt(), (yyvsp[-1].e))); }
#line 4293 "parser.c" /* yacc.c:1651  */
    break;

  case 323:
#line 457 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("disk"), OPT_DISK, NULL); }
#line 4299 "parser.c" /* yacc.c:1651  */
    break;

  case 324:
#line 458 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("files"), OPT_FILES, NULL); }
#line 4305 "parser.c" /* yacc.c:1651  */
    break;

  case 325:
#line 459 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("fonts"), OPT_FONTS, NULL); }
#line 4311 "parser.c" /* yacc.c:1651  */
    break;

  case 326:
#line 460 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getdefaulttool"), OPT_GETDEFAULTTOOL, push(new_contxt(), (yyvsp[-1].e))); }
#line 4317 "parser.c" /* yacc.c:1651  */
    break;

  case 327:
#line 461 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getposition"), OPT_GETPOSITION, (yyvsp[-1].e)); }
#line 4323 "parser.c" /* yacc.c:1651  */
    break;

  case 328:
#line 462 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getstack"), OPT_GETSTACK, push(new_contxt(), (yyvsp[-1].e))); }
#line 4329 "parser.c" /* yacc.c:1651  */
    break;

  case 329:
#line 463 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("gettooltype"), OPT_GETTOOLTYPE, (yyvsp[-1].e)); }
#line 4335 "parser.c" /* yacc.c:1651  */
    break;

  case 330:
#line 464 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("help"), OPT_HELP, (yyvsp[-1].e)); }
#line 4341 "parser.c" /* yacc.c:1651  */
    break;

  case 331:
#line 465 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("help"), OPT_HELP, push(new_contxt(), new_symref(strdup("@null"), LINE))); }
#line 4347 "parser.c" /* yacc.c:1651  */
    break;

  case 332:
#line 466 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("infos"), OPT_INFOS, NULL); }
#line 4353 "parser.c" /* yacc.c:1651  */
    break;

  case 333:
#line 467 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("include"), OPT_INCLUDE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4359 "parser.c" /* yacc.c:1651  */
    break;

  case 334:
#line 468 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("newname"), OPT_NEWNAME, push(new_contxt(), (yyvsp[-1].e))); }
#line 4365 "parser.c" /* yacc.c:1651  */
    break;

  case 335:
#line 469 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("newpath"), OPT_NEWPATH, NULL); }
#line 4371 "parser.c" /* yacc.c:1651  */
    break;

  case 336:
#line 470 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("nogauge"), OPT_NOGAUGE, NULL); }
#line 4377 "parser.c" /* yacc.c:1651  */
    break;

  case 337:
#line 471 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("noposition"), OPT_NOPOSITION, NULL); }
#line 4383 "parser.c" /* yacc.c:1651  */
    break;

  case 338:
#line 472 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("noreq"), OPT_NOREQ, NULL); }
#line 4389 "parser.c" /* yacc.c:1651  */
    break;

  case 339:
#line 473 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("pattern"), OPT_PATTERN, push(new_contxt(), (yyvsp[-1].e))); }
#line 4395 "parser.c" /* yacc.c:1651  */
    break;

  case 340:
#line 474 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("prompt"), OPT_PROMPT, (yyvsp[-1].e)); }
#line 4401 "parser.c" /* yacc.c:1651  */
    break;

  case 341:
#line 475 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("quiet"), OPT_QUIET, NULL); }
#line 4407 "parser.c" /* yacc.c:1651  */
    break;

  case 342:
#line 476 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("range"), OPT_RANGE, (yyvsp[-1].e)); }
#line 4413 "parser.c" /* yacc.c:1651  */
    break;

  case 343:
#line 477 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("safe"), OPT_SAFE, NULL); }
#line 4419 "parser.c" /* yacc.c:1651  */
    break;

  case 344:
#line 478 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setdefaulttool"), OPT_SETDEFAULTTOOL, push(new_contxt(), (yyvsp[-1].e))); }
#line 4425 "parser.c" /* yacc.c:1651  */
    break;

  case 345:
#line 479 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setposition"), OPT_SETPOSITION, (yyvsp[-1].e)); }
#line 4431 "parser.c" /* yacc.c:1651  */
    break;

  case 346:
#line 480 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setstack"), OPT_SETSTACK, push(new_contxt(), (yyvsp[-1].e))); }
#line 4437 "parser.c" /* yacc.c:1651  */
    break;

  case 347:
#line 481 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("settooltype"), OPT_SETTOOLTYPE, (yyvsp[-1].e)); }
#line 4443 "parser.c" /* yacc.c:1651  */
    break;

  case 348:
#line 482 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("settooltype"), OPT_SETTOOLTYPE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4449 "parser.c" /* yacc.c:1651  */
    break;

  case 349:
#line 483 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("source"), OPT_SOURCE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4455 "parser.c" /* yacc.c:1651  */
    break;

  case 350:
#line 484 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("swapcolors"), OPT_SWAPCOLORS, NULL); }
#line 4461 "parser.c" /* yacc.c:1651  */
    break;

  case 351:
#line 485 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("optional"), OPT_OPTIONAL, (yyvsp[-1].e)); }
#line 4467 "parser.c" /* yacc.c:1651  */
    break;

  case 352:
#line 486 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("resident"), OPT_RESIDENT, NULL); }
#line 4473 "parser.c" /* yacc.c:1651  */
    break;

  case 353:
#line 487 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("override"), OPT_OVERRIDE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4479 "parser.c" /* yacc.c:1651  */
    break;

  case 354:
#line 488 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("dynopt"), OPT_DYNOPT, push(push(push(new_contxt(), (yyvsp[-3].e)), (yyvsp[-2].e)), (yyvsp[-1].e))); }
#line 4485 "parser.c" /* yacc.c:1651  */
    break;


#line 4489 "parser.c" /* yacc.c:1651  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 490 "../src/parser.y" /* yacc.c:1910  */


