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

#line 85 "parser.c" /* yacc.c:339  */

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
    INT = 260,
    HEX = 261,
    BIN = 262,
    AND = 263,
    BITAND = 264,
    BITNOT = 265,
    BITOR = 266,
    BITXOR = 267,
    NOT = 268,
    IN = 269,
    OR = 270,
    SHIFTLEFT = 271,
    SHIFTRIGHT = 272,
    XOR = 273,
    EQ = 274,
    GT = 275,
    GTE = 276,
    LT = 277,
    LTE = 278,
    NEQ = 279,
    IF = 280,
    SELECT = 281,
    UNTIL = 282,
    WHILE = 283,
    TRACE = 284,
    RETRACE = 285,
    EXECUTE = 286,
    REXX = 287,
    RUN = 288,
    ABORT = 289,
    EXIT = 290,
    ONERROR = 291,
    TRAP = 292,
    REBOOT = 293,
    COPYFILES = 294,
    COPYLIB = 295,
    DELETE = 296,
    EXISTS = 297,
    FILEONLY = 298,
    FOREACH = 299,
    MAKEASSIGN = 300,
    MAKEDIR = 301,
    PROTECT = 302,
    STARTUP = 303,
    TEXTFILE = 304,
    TOOLTYPE = 305,
    TRANSCRIPT = 306,
    RENAME = 307,
    COMPLETE = 308,
    DEBUG = 309,
    MESSAGE = 310,
    USER = 311,
    WELCOME = 312,
    WORKING = 313,
    DATABASE = 314,
    EARLIER = 315,
    GETASSIGN = 316,
    GETDEVICE = 317,
    GETDISKSPACE = 318,
    GETENV = 319,
    GETSIZE = 320,
    GETSUM = 321,
    GETVERSION = 322,
    ICONINFO = 323,
    CUS = 324,
    DCL = 325,
    ASKBOOL = 326,
    ASKCHOICE = 327,
    ASKDIR = 328,
    ASKDISK = 329,
    ASKFILE = 330,
    ASKNUMBER = 331,
    ASKOPTIONS = 332,
    ASKSTRING = 333,
    CAT = 334,
    EXPANDPATH = 335,
    FMT = 336,
    PATHONLY = 337,
    PATMATCH = 338,
    STRLEN = 339,
    SUBSTR = 340,
    TACKON = 341,
    SET = 342,
    SYMBOLSET = 343,
    SYMBOLVAL = 344,
    OPENWBOBJECT = 345,
    SHOWWBOBJECT = 346,
    CLOSEWBOBJECT = 347,
    ALL = 348,
    APPEND = 349,
    ASSIGNS = 350,
    BACK = 351,
    CHOICES = 352,
    COMMAND = 353,
    COMPRESSION = 354,
    CONFIRM = 355,
    DEFAULT = 356,
    DELOPTS = 357,
    DEST = 358,
    DISK = 359,
    FILES = 360,
    FONTS = 361,
    GETDEFAULTTOOL = 362,
    GETPOSITION = 363,
    GETSTACK = 364,
    GETTOOLTYPE = 365,
    HELP = 366,
    INFOS = 367,
    INCLUDE = 368,
    NEWNAME = 369,
    NEWPATH = 370,
    NOGAUGE = 371,
    NOPOSITION = 372,
    NOREQ = 373,
    PATTERN = 374,
    PROMPT = 375,
    QUIET = 376,
    RANGE = 377,
    SAFE = 378,
    SETDEFAULTTOOL = 379,
    SETPOSITION = 380,
    SETSTACK = 381,
    SETTOOLTYPE = 382,
    SOURCE = 383,
    SWAPCOLORS = 384,
    OPTIONAL = 385,
    RESIDENT = 386,
    OVERRIDE = 387
  };
#endif
/* Tokens.  */
#define SYM 258
#define STR 259
#define INT 260
#define HEX 261
#define BIN 262
#define AND 263
#define BITAND 264
#define BITNOT 265
#define BITOR 266
#define BITXOR 267
#define NOT 268
#define IN 269
#define OR 270
#define SHIFTLEFT 271
#define SHIFTRIGHT 272
#define XOR 273
#define EQ 274
#define GT 275
#define GTE 276
#define LT 277
#define LTE 278
#define NEQ 279
#define IF 280
#define SELECT 281
#define UNTIL 282
#define WHILE 283
#define TRACE 284
#define RETRACE 285
#define EXECUTE 286
#define REXX 287
#define RUN 288
#define ABORT 289
#define EXIT 290
#define ONERROR 291
#define TRAP 292
#define REBOOT 293
#define COPYFILES 294
#define COPYLIB 295
#define DELETE 296
#define EXISTS 297
#define FILEONLY 298
#define FOREACH 299
#define MAKEASSIGN 300
#define MAKEDIR 301
#define PROTECT 302
#define STARTUP 303
#define TEXTFILE 304
#define TOOLTYPE 305
#define TRANSCRIPT 306
#define RENAME 307
#define COMPLETE 308
#define DEBUG 309
#define MESSAGE 310
#define USER 311
#define WELCOME 312
#define WORKING 313
#define DATABASE 314
#define EARLIER 315
#define GETASSIGN 316
#define GETDEVICE 317
#define GETDISKSPACE 318
#define GETENV 319
#define GETSIZE 320
#define GETSUM 321
#define GETVERSION 322
#define ICONINFO 323
#define CUS 324
#define DCL 325
#define ASKBOOL 326
#define ASKCHOICE 327
#define ASKDIR 328
#define ASKDISK 329
#define ASKFILE 330
#define ASKNUMBER 331
#define ASKOPTIONS 332
#define ASKSTRING 333
#define CAT 334
#define EXPANDPATH 335
#define FMT 336
#define PATHONLY 337
#define PATMATCH 338
#define STRLEN 339
#define SUBSTR 340
#define TACKON 341
#define SET 342
#define SYMBOLSET 343
#define SYMBOLVAL 344
#define OPENWBOBJECT 345
#define SHOWWBOBJECT 346
#define CLOSEWBOBJECT 347
#define ALL 348
#define APPEND 349
#define ASSIGNS 350
#define BACK 351
#define CHOICES 352
#define COMMAND 353
#define COMPRESSION 354
#define CONFIRM 355
#define DEFAULT 356
#define DELOPTS 357
#define DEST 358
#define DISK 359
#define FILES 360
#define FONTS 361
#define GETDEFAULTTOOL 362
#define GETPOSITION 363
#define GETSTACK 364
#define GETTOOLTYPE 365
#define HELP 366
#define INFOS 367
#define INCLUDE 368
#define NEWNAME 369
#define NEWPATH 370
#define NOGAUGE 371
#define NOPOSITION 372
#define NOREQ 373
#define PATTERN 374
#define PROMPT 375
#define QUIET 376
#define RANGE 377
#define SAFE 378
#define SETDEFAULTTOOL 379
#define SETPOSITION 380
#define SETSTACK 381
#define SETTOOLTYPE 382
#define SOURCE 383
#define SWAPCOLORS 384
#define OPTIONAL 385
#define RESIDENT 386
#define OVERRIDE 387

/* Value type.  */



int yyparse (yyscan_t scanner);

#endif /* !YY_YY_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 394 "parser.c" /* yacc.c:358  */

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
#define YYLAST   1986

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  142
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  149
/* YYNRULES -- Number of rules.  */
#define YYNRULES  351
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  679

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   387

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
     133,   134,   137,   135,     2,   138,     2,   136,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     141,   139,   140,     2,     2,     2,     2,     2,     2,     2,
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    96,    96,    97,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   267,   268,   269,   270,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     286,   287,   288,   289,   290,   291,   294,   295,   296,   297,
     298,   299,   300,   301,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   318,   319,   320,   321,
     322,   323,   324,   326,   327,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   363,   364,   365,   366,
     367,   368,   369,   370,   371,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   385,   386,   387,   388,
     391,   392,   393,   394,   395,   396,   399,   400,   401,   402,
     403,   404,   405,   406,   407,   408,   409,   410,   411,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   426,
     427,   428,   431,   432,   433,   434,   435,   436,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   465,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   482
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SYM", "STR", "INT", "HEX", "BIN", "AND",
  "BITAND", "BITNOT", "BITOR", "BITXOR", "NOT", "IN", "OR", "SHIFTLEFT",
  "SHIFTRIGHT", "XOR", "EQ", "GT", "GTE", "LT", "LTE", "NEQ", "IF",
  "SELECT", "UNTIL", "WHILE", "TRACE", "RETRACE", "EXECUTE", "REXX", "RUN",
  "ABORT", "EXIT", "ONERROR", "TRAP", "REBOOT", "COPYFILES", "COPYLIB",
  "DELETE", "EXISTS", "FILEONLY", "FOREACH", "MAKEASSIGN", "MAKEDIR",
  "PROTECT", "STARTUP", "TEXTFILE", "TOOLTYPE", "TRANSCRIPT", "RENAME",
  "COMPLETE", "DEBUG", "MESSAGE", "USER", "WELCOME", "WORKING", "DATABASE",
  "EARLIER", "GETASSIGN", "GETDEVICE", "GETDISKSPACE", "GETENV", "GETSIZE",
  "GETSUM", "GETVERSION", "ICONINFO", "CUS", "DCL", "ASKBOOL", "ASKCHOICE",
  "ASKDIR", "ASKDISK", "ASKFILE", "ASKNUMBER", "ASKOPTIONS", "ASKSTRING",
  "CAT", "EXPANDPATH", "FMT", "PATHONLY", "PATMATCH", "STRLEN", "SUBSTR",
  "TACKON", "SET", "SYMBOLSET", "SYMBOLVAL", "OPENWBOBJECT",
  "SHOWWBOBJECT", "CLOSEWBOBJECT", "ALL", "APPEND", "ASSIGNS", "BACK",
  "CHOICES", "COMMAND", "COMPRESSION", "CONFIRM", "DEFAULT", "DELOPTS",
  "DEST", "DISK", "FILES", "FONTS", "GETDEFAULTTOOL", "GETPOSITION",
  "GETSTACK", "GETTOOLTYPE", "HELP", "INFOS", "INCLUDE", "NEWNAME",
  "NEWPATH", "NOGAUGE", "NOPOSITION", "NOREQ", "PATTERN", "PROMPT",
  "QUIET", "RANGE", "SAFE", "SETDEFAULTTOOL", "SETPOSITION", "SETSTACK",
  "SETTOOLTYPE", "SOURCE", "SWAPCOLORS", "OPTIONAL", "RESIDENT",
  "OVERRIDE", "'('", "')'", "'+'", "'/'", "'*'", "'-'", "'='", "'>'",
  "'<'", "$accept", "start", "s", "p", "pp", "ps", "pps", "vp", "vps",
  "opts", "xpb", "xpbs", "np", "sps", "par", "cv", "cvv", "opt", "ivp",
  "add", "div", "mul", "sub", "and", "bitand", "bitnot", "bitor", "bitxor",
  "not", "in", "or", "shiftleft", "shiftright", "xor", "eq", "gt", "gte",
  "lt", "lte", "neq", "if", "select", "until", "while", "trace", "retrace",
  "execute", "rexx", "run", "abort", "exit", "onerror", "reboot", "trap",
  "copyfiles", "copylib", "delete", "exists", "fileonly", "foreach",
  "makeassign", "makedir", "protect", "startup", "textfile", "tooltype",
  "transcript", "rename", "complete", "debug", "message", "user",
  "welcome", "working", "database", "earlier", "getassign", "getdevice",
  "getdiskspace", "getenv", "getsize", "getsum", "getversion", "iconinfo",
  "dcl", "cus", "askbool", "askchoice", "askdir", "askdisk", "askfile",
  "asknumber", "askoptions", "askstring", "cat", "expandpath", "fmt",
  "pathonly", "patmatch", "strlen", "substr", "tackon", "set", "symbolset",
  "symbolval", "openwbobject", "showwbobject", "closewbobject", "all",
  "append", "assigns", "back", "choices", "command", "compression",
  "confirm", "default", "delopts", "dest", "disk", "files", "fonts",
  "getdefaulttool", "getposition", "getstack", "gettooltype", "help",
  "infos", "include", "newname", "newpath", "nogauge", "noposition",
  "noreq", "pattern", "prompt", "quiet", "range", "safe", "setdefaulttool",
  "setposition", "setstack", "settooltype", "source", "swapcolors",
  "optional", "resident", "override", "dynopt", YY_NULLPTR
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
     385,   386,   387,    40,    41,    43,    47,    42,    45,    61,
      62,    60
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
    -123,  1618,    42,  -543,  -543,  -123,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,    13,    18,     8,     8,
       8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,     8,   -86,   -75,   280,   280,
     280,     8,    23,  -123,     8,   -50,   -45,   -45,   280,     8,
       8,     8,     8,   280,     8,   280,   -45,   -45,     8,     8,
       8,    32,     8,     8,    40,     8,     8,     8,     8,     8,
       8,     8,     8,     8,    46,   -45,    94,  -102,   -45,  -100,
     -45,   -93,   -51,   -45,   -47,     8,     8,     8,     8,     8,
       8,     8,    96,     8,     8,   280,     8,     8,     8,     8,
       8,     8,     8,     8,     8,   -29,   -31,  -543,  -123,  -543,
    -543,  -543,  -543,  -543,  1757,  -543,  -543,    51,  -543,  -543,
    -543,    74,   115,     8,   -10,    -1,    16,    21,    41,     8,
     194,    43,    98,   100,   107,   116,   130,   199,   142,   144,
     796,  -123,  -123,  -543,  -543,   923,   206,   280,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
     211,   280,   223,   280,   239,  1201,  -543,   315,   414,   -17,
    -123,  -543,  1813,    15,    20,    26,   280,    34,   147,  -123,
     431,    36,    49,   280,   436,    57,   -45,   460,    59,    62,
     473,    86,   154,  -543,   489,   494,   156,  -543,   499,   521,
     534,   164,   171,   568,   182,   189,   595,   191,   197,   200,
     204,  -543,   106,   119,   103,  -543,   126,   129,  -543,   134,
     139,  -543,   159,  -543,   161,   163,  -543,   167,   653,   209,
     215,   226,   229,   674,   233,     8,     5,  -543,   685,   247,
     169,   280,   258,   318,   690,   336,   700,   352,   353,   355,
     356,  -543,  -543,   -29,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,   714,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  1618,  -543,  -543,   796,  -543,  -543,  -543,   719,
     174,   177,     8,   373,     8,   374,  -123,     8,     8,   376,
     737,     8,     8,     8,   379,   381,   382,     8,     8,     8,
       8,   742,   385,     8,     8,   386,   395,   396,   397,     8,
       8,   398,     8,   400,     8,     8,     8,     8,     8,   410,
       8,   412,     8,   180,  -543,   196,   750,   -45,  -543,   203,
     755,  -543,   208,   764,  -543,  -543,   416,  -543,   771,  -543,
     214,     8,  -543,  -543,  -543,   218,   221,   312,  -543,   417,
    -543,   225,  1340,  -543,   418,   439,  -543,   420,  -543,   232,
     243,  -543,   245,  -543,   261,   266,  -543,   269,  -543,  -543,
    -543,  -543,   272,  -543,  -543,  1479,  -543,   427,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,   432,  -543,   433,  -543,  -543,  -543,   443,   123,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,   450,  -543,  -543,     8,  -543,  -543,
       8,  -543,  -543,   274,   277,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,   776,  -543,  -543,  -543,  -543,
    -543,   785,  -543,   790,  -543,   292,   809,  1300,  -543,  -543,
     451,   452,  1305,   453,  -543,  -543,  -543,   455,   461,   462,
     474,  -543,  1310,  -543,   481,   483,  -543,  -543,  -543,  -543,
     485,  1320,  -543,   486,  -543,   487,   490,   491,  1325,   492,
     495,  -543,  1442,  -543,   496,  -543,  -543,  -543,   294,  -543,
    -543,   311,  -543,  -543,   313,  -543,  -543,  -543,   498,  -543,
    -543,   335,  -543,  -543,  -543,  -543,  -543,  -543,   339,  -543,
    -543,  -543,  -543,   341,  -543,  -543,  -543,  -543,  -543,  -543,
     502,  -543,  -543,  -543,  -543,  -543,   348,  -543,   504,  1062,
     498,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    1854,  -543,  -543,  -543,  -543,  -543,  -543,   506,  -543
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     2,    14,     3,    11,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     110,   109,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   142,   141,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    14,     0,     1,    13,    28,
      27,    24,    25,    26,     0,   275,     8,     0,     4,     5,
     292,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   192,   193,     0,     0,     0,    17,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    55,    54,
      56,    57,    58,    59,    60,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    61,    75,    62,    74,
       0,     0,     0,     0,     0,     0,   211,     0,     0,     0,
       0,   213,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   248,     0,     0,     0,   253,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   266,     0,     0,     0,   276,     0,     0,   279,     0,
       0,   282,     0,   284,     0,     0,   287,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,    15,     0,   274,     7,   291,   169,     6,   170,
     171,   172,   173,   174,     0,   176,   177,   178,   179,   182,
     184,   185,     0,   188,    21,    33,   187,   186,    22,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,    16,   201,     0,
       0,   205,     0,     0,   206,   210,     0,   209,     0,   212,
       0,     0,   215,   216,   220,     0,     0,     0,   221,     0,
     223,     0,     0,   228,     0,     0,   226,     0,   232,     0,
       0,   236,     0,   234,     0,     0,   240,     0,   241,   242,
     243,   245,     0,   246,   247,     0,   250,     0,   251,   252,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,     0,   267,     0,   269,    32,   273,     0,     0,
     277,   278,   280,   281,   283,   285,   286,   288,   289,   290,
     293,   294,   295,   296,     0,   298,    30,     0,   299,   300,
       0,   301,   302,     0,     0,   306,   307,   165,   166,   167,
     168,   180,   181,   183,   175,     0,    34,   189,    23,   190,
     191,     0,   308,     0,   310,     0,     0,     0,   314,   316,
       0,     0,     0,     0,   320,   321,   322,     0,     0,     0,
       0,   328,     0,   329,     0,     0,   332,   333,   334,   335,
       0,     0,   338,     0,   340,     0,     0,     0,     0,     0,
       0,   347,     0,   349,     0,    18,   194,   195,     0,   198,
     199,     0,   202,   203,     0,   207,   208,   214,     0,   217,
     218,     0,   222,   224,   227,   225,   229,   230,     0,   235,
     233,   237,   238,     0,   244,   249,   268,   272,    31,   271,
       0,   297,    29,     9,   303,   304,     0,    19,     0,     0,
       0,   309,   311,   312,   313,   315,   317,   318,   319,   323,
     324,   325,   326,   327,   330,   331,   336,   337,   339,   341,
     342,   343,   345,   344,   346,   348,   350,   196,   200,   204,
       0,   219,   231,   239,   270,   305,    20,     0,   351
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -543,  -543,  -320,   -39,   503,   438,  -543,     0,     2,   296,
    -213,  -543,    99,  -543,  -543,  -543,  -543,  -542,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,   338,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,  -543,
    -543,  -543,  -543,   358,  -543,  -543,  -116,  -543,   193,  -543,
    -543,  -543,  -543,  -543,  -543,  -543,   326,  -543,  -543
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   196,   204,   197,   348,   198,   188,   437,
     385,   389,   199,   346,   509,   218,   219,   228,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
       4,   185,     5,   186,   508,     4,   278,   388,   527,   640,
       1,   189,   190,   191,   192,   193,   189,   190,   191,   192,
     193,   189,   190,   191,   192,   193,   189,   190,   191,   192,
     193,   282,   325,   282,   328,   189,   190,   191,   192,   193,
     282,   331,   187,   189,   190,   191,   192,   193,   223,   189,
     190,   191,   192,   193,   189,   190,   191,   192,   193,   224,
     203,   205,   203,   203,   208,   209,   640,   203,   203,   203,
     203,   203,   203,   217,   220,   221,   222,   189,   190,   191,
     192,   193,   282,   333,   281,   280,   282,   336,   282,   285,
     287,   288,   203,   290,   292,   294,   296,   324,   677,   345,
     203,   302,     1,   362,   306,   361,   506,   310,   203,   313,
     315,   316,   318,   319,   320,   322,     1,   449,   189,   190,
     191,   192,   193,     4,   369,   279,   628,   339,   340,   203,
     342,   203,   203,   370,   203,   349,   350,   352,   353,   528,
     203,   194,   203,   203,   203,   203,   194,   195,   282,   452,
     371,   194,   200,   282,   453,   372,   275,   276,   365,   282,
     454,   446,   365,   365,   368,   194,   303,   457,   458,   465,
     466,   365,   546,   194,   307,   373,   548,   376,   384,   194,
     321,   384,   282,   468,   194,   364,     4,   365,     4,   630,
     282,   473,   282,   478,   363,   282,   479,   189,   190,   191,
     192,   193,   189,   190,   191,   192,   193,   194,   366,   189,
     190,   191,   192,   193,   189,   190,   191,   192,   193,   282,
     481,     4,     4,   390,   391,   363,   189,   190,   191,   192,
     193,   365,   377,   365,   378,   365,     1,   507,   365,   502,
     503,   379,   189,   190,   191,   192,   193,   456,   194,   367,
     380,   368,   282,   505,   470,   368,     1,   629,   477,   282,
     510,   365,   282,   511,   381,   365,   365,   282,   512,   365,
     365,   368,   282,   513,   368,   363,   386,   368,   387,     4,
       4,   460,   450,   189,   190,   191,   192,   193,   483,     4,
     488,   461,   282,   514,   282,   515,   282,   516,   492,   365,
     282,   517,   282,   532,   524,   493,   526,     1,   549,   530,
       1,   550,   534,   282,   595,   365,   495,   365,   189,   190,
     191,   192,   193,   496,     4,   498,     5,   194,   375,   282,
     596,   499,   382,   383,   500,   365,   282,   599,   501,   225,
     434,   282,   602,   519,   225,   438,   384,     1,   607,   520,
     384,   282,   609,   551,   282,   610,   225,   441,     1,   613,
     521,   560,   561,   522,   563,   282,   616,   525,   567,   203,
     569,   203,   194,   444,   574,   575,   282,   617,   282,   619,
     580,   531,   185,   203,   545,   585,   203,   587,   588,   590,
       4,     4,   535,   594,   282,   620,     4,   365,   555,   282,
     621,   365,   282,   622,   365,   282,   624,   282,   634,   365,
     282,   635,   608,   225,   227,   271,   273,   189,   190,   191,
     192,   193,   283,   284,   286,     1,   642,   282,   667,   293,
     418,   297,   298,   299,   189,   190,   191,   192,   193,   189,
     190,   191,   192,   193,   282,   668,   282,   669,   275,   445,
       4,   323,   536,   326,   327,   329,   330,   332,   334,   335,
     337,     4,   363,   189,   190,   191,   192,   193,   282,   671,
     538,   351,   282,   672,   282,   673,   189,   190,   191,   192,
     193,   282,   675,   464,   467,   363,   540,   541,   632,   542,
     543,   633,   189,   190,   191,   192,   193,   189,   190,   191,
     192,   193,   189,   190,   191,   192,   193,   552,   554,     4,
     558,     5,   384,   564,   365,   565,   566,   365,   365,   573,
     576,   433,   435,   365,   189,   190,   191,   192,   193,   577,
     578,   579,   582,   365,   584,   201,   202,   189,   190,   191,
     192,   193,   365,   210,   591,     4,   593,   194,   447,   368,
     605,   612,   614,   365,   615,     4,   226,   270,   272,   274,
     277,   625,   423,   431,   462,   463,   439,   626,   442,   225,
     471,   189,   190,   191,   192,   193,   300,   627,   433,   304,
     305,   455,   308,   309,   631,   645,   646,   648,   469,   649,
     472,   474,   475,   225,   476,   650,   651,   482,   189,   190,
     191,   192,   193,   338,   206,   207,   194,   480,   652,   211,
     212,   213,   214,   215,   216,   654,   354,   655,   356,   656,
     658,   659,   194,   484,   660,   661,   663,   485,   486,   664,
     666,   670,   194,   489,   289,   291,   674,   295,   676,   185,
     678,   545,   301,   487,   638,   459,   533,   374,   504,   311,
     312,   314,     0,   317,   194,   490,   189,   190,   191,   192,
     193,     0,     0,     0,     0,   436,     0,   194,   491,     0,
       0,   341,     0,   343,   344,     0,   347,   189,   190,   191,
     192,   193,   355,     0,   357,   358,   359,   360,   189,   190,
     191,   192,   193,   189,   190,   191,   192,   193,     0,     0,
       0,   194,   494,   189,   190,   191,   192,   193,     0,   440,
       0,   443,     0,     0,     0,     0,   448,   189,   190,   191,
     192,   193,   189,   190,   191,   192,   193,     0,   194,   497,
       0,     0,   598,     0,     0,     0,   601,     0,     0,   604,
     189,   190,   191,   192,   193,   189,   190,   191,   192,   193,
       0,     0,   611,   189,   190,   191,   192,   193,   189,   190,
     191,   192,   193,     0,     0,     0,   618,   189,   190,   191,
     192,   193,     0,   623,   189,   190,   191,   192,   193,   189,
     190,   191,   192,   193,     0,     0,   194,   518,   189,   190,
     191,   192,   193,   189,   190,   191,   192,   193,     0,   189,
     190,   191,   192,   193,     0,     0,     0,   194,   523,     0,
       0,     0,   189,   190,   191,   192,   193,     0,   194,   529,
       0,     0,     0,   194,   537,     0,     0,     0,     0,     0,
     636,     0,   553,   194,   539,   556,   557,     0,     0,     0,
     562,     0,     0,     0,     0,     0,     0,   194,   544,   572,
       0,     0,   382,   547,     0,     0,     0,     0,   581,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   592,     0,
     194,   559,     0,     0,     0,   194,   571,     0,     0,     0,
       0,     0,     0,   225,   597,     0,     0,     0,   225,   600,
       0,     0,     0,     0,     0,     0,     0,   225,   603,     0,
       0,     0,     0,     0,   194,   606,     0,     0,     0,     1,
     637,   568,     0,   570,     0,     0,     0,     0,   639,   383,
       0,     0,     0,   194,   641,   583,    96,    97,   586,   382,
     589,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   194,   643,   109,     0,   110,   111,   392,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,     0,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   403,   404,   405,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   431,   432,   225,     0,   178,   179,
     180,   181,   182,   183,   184,    96,    97,     0,     0,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,     0,     0,   109,     0,   110,   111,   392,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,     0,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,     0,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,   407,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   427,
     428,   429,   430,   431,   432,     1,     0,   178,   179,   180,
     181,   182,   183,   184,    96,    97,     0,     0,     0,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
       0,     0,   109,     0,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
       0,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,     0,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   189,   190,   191,   192,   193,   189,   190,
     191,   192,   193,   189,   190,   191,   192,   193,     0,     0,
       0,     0,   421,   189,   190,   191,   192,   193,   189,   190,
     191,   192,   193,     0,   194,     0,   178,   179,   180,   181,
     182,   183,   184,    96,    97,     0,     0,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,     0,
       0,   109,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,     0,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,     0,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   194,   644,     0,     0,     0,   194,   647,
       0,     0,     0,   194,   653,   189,   190,   191,   192,   193,
       0,     0,     0,   194,   657,     0,     0,     0,   194,   662,
       0,     0,     0,   423,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   194,     0,   178,   179,   180,   181,   182,
     183,   184,    96,    97,     0,     0,     0,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,     0,     0,
     109,     0,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,     0,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
       0,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   393,     0,     0,   194,   665,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194,     0,   178,   179,   180,   181,   182,   183,
     184,    96,    97,     0,     0,     0,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,   109,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,     0,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,     0,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     1,     0,   178,   179,   180,   181,   182,   183,   184,
      96,    97,     0,     0,     0,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,     0,     0,   109,     0,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   451,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   451,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,     0,   178,   179,   180,   181,   182,   183,   184,     0,
       0,     0,     0,     0,     0,     0,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   403,   404,   405,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   431,   432,   282,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432
};

static const yytype_int16 yycheck[] =
{
       0,     1,     0,     1,   324,     5,   122,   220,     3,   551,
     133,     3,     4,     5,     6,     7,     3,     4,     5,     6,
       7,     3,     4,     5,     6,     7,     3,     4,     5,     6,
       7,   133,   134,   133,   134,     3,     4,     5,     6,     7,
     133,   134,     0,     3,     4,     5,     6,     7,   134,     3,
       4,     5,     6,     7,     3,     4,     5,     6,     7,   134,
      99,   100,   101,   102,   103,   104,   608,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,     3,     4,     5,
       6,     7,   133,   134,   134,   124,   133,   134,   133,   128,
     129,   130,   131,   132,   133,   134,   135,     3,   640,     3,
     139,   140,   133,   134,   143,   134,     3,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   133,   134,     3,     4,
       5,     6,     7,   123,   134,   123,     3,   166,   167,   168,
     169,   170,   171,   134,   173,   174,   175,   176,   177,   134,
     179,   133,   181,   182,   183,   184,   133,   134,   133,   134,
     134,   133,   134,   133,   134,   134,   133,   134,   197,   133,
     134,   277,   201,   202,   203,   133,   134,   133,   134,   133,
     134,   210,   385,   133,   134,   134,   389,   134,   217,   133,
     134,   220,   133,   134,   133,   134,   186,   226,   188,   509,
     133,   134,   133,   134,   194,   133,   134,     3,     4,     5,
       6,     7,     3,     4,     5,     6,     7,   133,   134,     3,
       4,     5,     6,     7,     3,     4,     5,     6,     7,   133,
     134,   221,   222,   221,   222,   225,     3,     4,     5,     6,
       7,   270,   134,   272,   134,   274,   133,   134,   277,   133,
     134,   134,     3,     4,     5,     6,     7,   286,   133,   134,
     134,   290,   133,   134,   293,   294,   133,   134,   297,   133,
     134,   300,   133,   134,   134,   304,   305,   133,   134,   308,
     309,   310,   133,   134,   313,   275,   134,   316,   134,   279,
     280,   134,   280,     3,     4,     5,     6,     7,   134,   289,
     134,   289,   133,   134,   133,   134,   133,   134,   134,   338,
     133,   134,   133,   134,   343,   134,   345,   133,   134,   348,
     133,   134,   351,   133,   134,   354,   134,   356,     3,     4,
       5,     6,     7,   134,   324,   134,   324,   133,   134,   133,
     134,   134,   133,   134,   134,   374,   133,   134,   134,   133,
     134,   133,   134,   134,   133,   134,   385,   133,   134,   134,
     389,   133,   134,   392,   133,   134,   133,   134,   133,   134,
     134,   400,   401,   134,   403,   133,   134,   134,   407,   408,
     409,   410,   133,   134,   413,   414,   133,   134,   133,   134,
     419,   134,   382,   422,   382,   424,   425,   426,   427,   428,
     390,   391,   134,   432,   133,   134,   396,   436,   396,   133,
     134,   440,   133,   134,   443,   133,   134,   133,   134,   448,
     133,   134,   451,   133,   118,   119,   120,     3,     4,     5,
       6,     7,   126,   127,   128,   133,   134,   133,   134,   133,
     118,   135,   136,   137,     3,     4,     5,     6,     7,     3,
       4,     5,     6,     7,   133,   134,   133,   134,   133,   134,
     450,   155,   134,   157,   158,   159,   160,   161,   162,   163,
     164,   461,   462,     3,     4,     5,     6,     7,   133,   134,
     134,   175,   133,   134,   133,   134,     3,     4,     5,     6,
       7,   133,   134,   290,   291,   485,   134,   134,   527,   134,
     134,   530,     3,     4,     5,     6,     7,     3,     4,     5,
       6,     7,     3,     4,     5,     6,     7,   134,   134,   509,
     134,   509,   551,   134,   553,   134,   134,   556,   557,   134,
     134,   225,   226,   562,     3,     4,     5,     6,     7,   134,
     134,   134,   134,   572,   134,    97,    98,     3,     4,     5,
       6,     7,   581,   105,   134,   545,   134,   133,   134,   588,
     134,   134,   134,   592,   134,   555,   118,   119,   120,   121,
     122,   134,   123,   131,   133,   134,   270,   134,   272,   133,
     134,     3,     4,     5,     6,     7,   138,   134,   282,   141,
     142,   285,   144,   145,   134,   134,   134,   134,   292,   134,
     294,   295,   296,   133,   134,   134,   134,   301,     3,     4,
       5,     6,     7,   165,   101,   102,   133,   134,   134,   106,
     107,   108,   109,   110,   111,   134,   178,   134,   180,   134,
     134,   134,   133,   134,   134,   134,   134,   133,   134,   134,
     134,   133,   133,   134,   131,   132,   134,   134,   134,   639,
     134,   639,   139,   305,   545,   287,   350,   209,   322,   146,
     147,   148,    -1,   150,   133,   134,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    -1,   227,    -1,   133,   134,    -1,
      -1,   168,    -1,   170,   171,    -1,   173,     3,     4,     5,
       6,     7,   179,    -1,   181,   182,   183,   184,     3,     4,
       5,     6,     7,     3,     4,     5,     6,     7,    -1,    -1,
      -1,   133,   134,     3,     4,     5,     6,     7,    -1,   271,
      -1,   273,    -1,    -1,    -1,    -1,   278,     3,     4,     5,
       6,     7,     3,     4,     5,     6,     7,    -1,   133,   134,
      -1,    -1,   436,    -1,    -1,    -1,   440,    -1,    -1,   443,
       3,     4,     5,     6,     7,     3,     4,     5,     6,     7,
      -1,    -1,   456,     3,     4,     5,     6,     7,     3,     4,
       5,     6,     7,    -1,    -1,    -1,   470,     3,     4,     5,
       6,     7,    -1,   477,     3,     4,     5,     6,     7,     3,
       4,     5,     6,     7,    -1,    -1,   133,   134,     3,     4,
       5,     6,     7,     3,     4,     5,     6,     7,    -1,     3,
       4,     5,     6,     7,    -1,    -1,    -1,   133,   134,    -1,
      -1,    -1,     3,     4,     5,     6,     7,    -1,   133,   134,
      -1,    -1,    -1,   133,   134,    -1,    -1,    -1,    -1,    -1,
     534,    -1,   394,   133,   134,   397,   398,    -1,    -1,    -1,
     402,    -1,    -1,    -1,    -1,    -1,    -1,   133,   134,   411,
      -1,    -1,   133,   134,    -1,    -1,    -1,    -1,   420,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   430,    -1,
     133,   134,    -1,    -1,    -1,   133,   134,    -1,    -1,    -1,
      -1,    -1,    -1,   133,   134,    -1,    -1,    -1,   133,   134,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,   134,    -1,
      -1,    -1,    -1,    -1,   133,   134,    -1,    -1,    -1,   133,
     134,   408,    -1,   410,    -1,    -1,    -1,    -1,   133,   134,
      -1,    -1,    -1,   133,   134,   422,     3,     4,   425,   133,
     427,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,   133,   134,    21,    -1,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,    -1,   135,   136,
     137,   138,   139,   140,   141,     3,     4,    -1,    -1,    -1,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    -1,    -1,    21,    -1,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,    -1,   135,   136,   137,
     138,   139,   140,   141,     3,     4,    -1,    -1,    -1,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    21,    -1,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      -1,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,     5,     6,     7,     3,     4,
       5,     6,     7,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,   121,     3,     4,     5,     6,     7,     3,     4,
       5,     6,     7,    -1,   133,    -1,   135,   136,   137,   138,
     139,   140,   141,     3,     4,    -1,    -1,    -1,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    -1,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    -1,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,   133,   134,    -1,    -1,    -1,   133,   134,
      -1,    -1,    -1,   133,   134,     3,     4,     5,     6,     7,
      -1,    -1,    -1,   133,   134,    -1,    -1,    -1,   133,   134,
      -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   133,    -1,   135,   136,   137,   138,   139,
     140,   141,     3,     4,    -1,    -1,    -1,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      21,    -1,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    -1,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    -1,    -1,   133,   134,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   133,    -1,   135,   136,   137,   138,   139,   140,
     141,     3,     4,    -1,    -1,    -1,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    -1,    -1,    21,
      -1,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    -1,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    -1,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   133,    -1,   135,   136,   137,   138,   139,   140,   141,
       3,     4,    -1,    -1,    -1,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    25,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     133,    -1,   135,   136,   137,   138,   139,   140,   141,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   133,   143,   144,   149,   150,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,     3,     4,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    21,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,   135,   136,
     137,   138,   139,   140,   141,   149,   150,     0,   150,     3,
       4,     5,     6,     7,   133,   134,   145,   147,   149,   154,
     134,   147,   147,   145,   146,   145,   146,   146,   145,   145,
     147,   146,   146,   146,   146,   146,   146,   145,   157,   158,
     145,   145,   145,   134,   134,   133,   147,   151,   159,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     147,   151,   147,   151,   147,   133,   134,   147,   278,   150,
     145,   134,   133,   151,   151,   145,   151,   145,   145,   146,
     145,   146,   145,   151,   145,   146,   145,   151,   151,   151,
     147,   146,   145,   134,   147,   147,   145,   134,   147,   147,
     145,   146,   146,   145,   146,   145,   145,   146,   145,   145,
     145,   134,   145,   151,     3,   134,   151,   151,   134,   151,
     151,   134,   151,   134,   151,   151,   134,   151,   147,   145,
     145,   146,   145,   146,   146,     3,   155,   146,   148,   145,
     145,   151,   145,   145,   147,   146,   147,   146,   146,   146,
     146,   134,   134,   149,   134,   145,   134,   134,   145,   134,
     134,   134,   134,   134,   147,   134,   134,   134,   134,   134,
     134,   134,   133,   134,   145,   152,   134,   134,   152,   153,
     150,   150,    25,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   151,   134,   151,   147,   151,   134,   151,
     147,   134,   151,   147,   134,   134,   278,   134,   147,   134,
     150,    25,   134,   134,   134,   151,   145,   133,   134,   275,
     134,   150,   133,   134,   280,   133,   134,   280,   134,   151,
     145,   134,   151,   134,   151,   151,   134,   145,   134,   134,
     134,   134,   151,   134,   134,   133,   134,   250,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   133,   134,   288,   134,     3,   134,   144,   156,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   145,   134,   145,     3,   134,   134,
     145,   134,   134,   151,   145,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   150,   152,   134,   152,   134,
     134,   145,   134,   147,   134,   150,   147,   147,   134,   134,
     145,   145,   147,   145,   134,   134,   134,   145,   146,   145,
     146,   134,   147,   134,   145,   145,   134,   134,   134,   134,
     145,   147,   134,   146,   134,   145,   146,   145,   145,   146,
     145,   134,   147,   134,   145,   134,   134,   134,   151,   134,
     134,   151,   134,   134,   151,   134,   134,   134,   145,   134,
     134,   151,   134,   134,   134,   134,   134,   134,   151,   134,
     134,   134,   134,   151,   134,   134,   134,   134,     3,   134,
     144,   134,   145,   145,   134,   134,   151,   134,   154,   133,
     159,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     133,   134,   134,   134,   134,   134,   134,   159,   134
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   142,   143,   144,   145,   145,   146,   147,   147,   148,
     148,   149,   149,   150,   150,   150,   151,   151,   151,   152,
     152,   152,   153,   153,   154,   154,   154,   154,   154,   155,
     155,   156,   156,   157,   158,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   182,   182,   183,
     184,   185,   186,   187,   188,   188,   188,   188,   189,   189,
     189,   189,   190,   190,   190,   190,   191,   192,   192,   192,
     192,   192,   193,   194,   195,   196,   197,   198,   198,   198,
     198,   199,   199,   200,   201,   202,   202,   202,   202,   203,
     203,   203,   203,   204,   204,   204,   204,   205,   205,   205,
     205,   206,   207,   208,   209,   209,   210,   211,   211,   212,
     212,   213,   214,   214,   215,   216,   216,   217,   218,   218,
     219,   220,   220,   221,   222,   223,   224,   224,   224,   225,
     226,   226,   226,   226,   227,   227,   228,   228,   229,   230,
     230,   231,   232,   232,   233,   233,   234,   235,   235,   236,
     237,   238,   238,   239,   240,   241,   242,   242,   243,   244,
     245,   246,   247,   247,   247,   247,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   284,   285,   286,   287,   288,
     289,   290
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     2,     2,     1,     3,
       1,     1,     3,     2,     1,     3,     2,     1,     3,     3,
       4,     1,     1,     2,     1,     1,     1,     1,     1,     3,
       2,     2,     1,     2,     3,     1,     1,     1,     1,     1,
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
       1,     1,     1,     1,     1,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     5,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     5,
       5,     5,     3,     3,     5,     5,     6,     4,     5,     5,
       6,     4,     5,     5,     6,     4,     4,     5,     5,     4,
       4,     3,     4,     3,     5,     4,     4,     5,     5,     6,
       4,     4,     5,     4,     5,     5,     4,     5,     4,     5,
       5,     6,     4,     5,     4,     5,     4,     5,     5,     6,
       4,     4,     4,     4,     5,     4,     4,     4,     3,     5,
       4,     4,     4,     3,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     3,     4,     5,     4,
       6,     5,     5,     4,     4,     3,     3,     4,     4,     3,
       4,     4,     3,     4,     3,     4,     4,     3,     4,     4,
       4,     4,     3,     4,     4,     4,     4,     5,     4,     4,
       4,     4,     4,     5,     5,     6,     4,     4,     3,     4,
       3,     4,     4,     4,     3,     4,     3,     4,     4,     4,
       3,     3,     3,     4,     4,     4,     4,     4,     3,     3,
       4,     4,     3,     3,     3,     3,     4,     4,     3,     4,
       3,     4,     4,     4,     4,     4,     4,     3,     4,     3,
       4,     6
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
#line 77 "../src/parser.y" /* yacc.c:1258  */
      { free(((*yyvaluep).s)); }
#line 2007 "parser.c" /* yacc.c:1258  */
        break;

    case 4: /* STR  */
#line 77 "../src/parser.y" /* yacc.c:1258  */
      { free(((*yyvaluep).s)); }
#line 2013 "parser.c" /* yacc.c:1258  */
        break;

    case 143: /* start  */
#line 75 "../src/parser.y" /* yacc.c:1258  */
      { run(((*yyvaluep).e));  }
#line 2019 "parser.c" /* yacc.c:1258  */
        break;

    case 144: /* s  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2025 "parser.c" /* yacc.c:1258  */
        break;

    case 145: /* p  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2031 "parser.c" /* yacc.c:1258  */
        break;

    case 146: /* pp  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2037 "parser.c" /* yacc.c:1258  */
        break;

    case 147: /* ps  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2043 "parser.c" /* yacc.c:1258  */
        break;

    case 148: /* pps  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2049 "parser.c" /* yacc.c:1258  */
        break;

    case 149: /* vp  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2055 "parser.c" /* yacc.c:1258  */
        break;

    case 150: /* vps  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2061 "parser.c" /* yacc.c:1258  */
        break;

    case 151: /* opts  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2067 "parser.c" /* yacc.c:1258  */
        break;

    case 152: /* xpb  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2073 "parser.c" /* yacc.c:1258  */
        break;

    case 153: /* xpbs  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2079 "parser.c" /* yacc.c:1258  */
        break;

    case 154: /* np  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2085 "parser.c" /* yacc.c:1258  */
        break;

    case 155: /* sps  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2091 "parser.c" /* yacc.c:1258  */
        break;

    case 156: /* par  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2097 "parser.c" /* yacc.c:1258  */
        break;

    case 157: /* cv  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2103 "parser.c" /* yacc.c:1258  */
        break;

    case 158: /* cvv  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2109 "parser.c" /* yacc.c:1258  */
        break;

    case 159: /* opt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2115 "parser.c" /* yacc.c:1258  */
        break;

    case 160: /* ivp  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2121 "parser.c" /* yacc.c:1258  */
        break;

    case 161: /* add  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2127 "parser.c" /* yacc.c:1258  */
        break;

    case 162: /* div  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2133 "parser.c" /* yacc.c:1258  */
        break;

    case 163: /* mul  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2139 "parser.c" /* yacc.c:1258  */
        break;

    case 164: /* sub  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2145 "parser.c" /* yacc.c:1258  */
        break;

    case 165: /* and  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2151 "parser.c" /* yacc.c:1258  */
        break;

    case 166: /* bitand  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2157 "parser.c" /* yacc.c:1258  */
        break;

    case 167: /* bitnot  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2163 "parser.c" /* yacc.c:1258  */
        break;

    case 168: /* bitor  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2169 "parser.c" /* yacc.c:1258  */
        break;

    case 169: /* bitxor  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2175 "parser.c" /* yacc.c:1258  */
        break;

    case 170: /* not  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2181 "parser.c" /* yacc.c:1258  */
        break;

    case 171: /* in  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2187 "parser.c" /* yacc.c:1258  */
        break;

    case 172: /* or  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2193 "parser.c" /* yacc.c:1258  */
        break;

    case 173: /* shiftleft  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2199 "parser.c" /* yacc.c:1258  */
        break;

    case 174: /* shiftright  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2205 "parser.c" /* yacc.c:1258  */
        break;

    case 175: /* xor  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2211 "parser.c" /* yacc.c:1258  */
        break;

    case 176: /* eq  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2217 "parser.c" /* yacc.c:1258  */
        break;

    case 177: /* gt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2223 "parser.c" /* yacc.c:1258  */
        break;

    case 178: /* gte  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2229 "parser.c" /* yacc.c:1258  */
        break;

    case 179: /* lt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2235 "parser.c" /* yacc.c:1258  */
        break;

    case 180: /* lte  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2241 "parser.c" /* yacc.c:1258  */
        break;

    case 181: /* neq  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2247 "parser.c" /* yacc.c:1258  */
        break;

    case 182: /* if  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2253 "parser.c" /* yacc.c:1258  */
        break;

    case 183: /* select  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2259 "parser.c" /* yacc.c:1258  */
        break;

    case 184: /* until  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2265 "parser.c" /* yacc.c:1258  */
        break;

    case 185: /* while  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2271 "parser.c" /* yacc.c:1258  */
        break;

    case 186: /* trace  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2277 "parser.c" /* yacc.c:1258  */
        break;

    case 187: /* retrace  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2283 "parser.c" /* yacc.c:1258  */
        break;

    case 188: /* execute  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2289 "parser.c" /* yacc.c:1258  */
        break;

    case 189: /* rexx  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2295 "parser.c" /* yacc.c:1258  */
        break;

    case 190: /* run  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2301 "parser.c" /* yacc.c:1258  */
        break;

    case 191: /* abort  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2307 "parser.c" /* yacc.c:1258  */
        break;

    case 192: /* exit  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2313 "parser.c" /* yacc.c:1258  */
        break;

    case 193: /* onerror  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2319 "parser.c" /* yacc.c:1258  */
        break;

    case 194: /* reboot  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2325 "parser.c" /* yacc.c:1258  */
        break;

    case 195: /* trap  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2331 "parser.c" /* yacc.c:1258  */
        break;

    case 196: /* copyfiles  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2337 "parser.c" /* yacc.c:1258  */
        break;

    case 197: /* copylib  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2343 "parser.c" /* yacc.c:1258  */
        break;

    case 198: /* delete  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2349 "parser.c" /* yacc.c:1258  */
        break;

    case 199: /* exists  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2355 "parser.c" /* yacc.c:1258  */
        break;

    case 200: /* fileonly  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2361 "parser.c" /* yacc.c:1258  */
        break;

    case 201: /* foreach  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2367 "parser.c" /* yacc.c:1258  */
        break;

    case 202: /* makeassign  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2373 "parser.c" /* yacc.c:1258  */
        break;

    case 203: /* makedir  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2379 "parser.c" /* yacc.c:1258  */
        break;

    case 204: /* protect  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2385 "parser.c" /* yacc.c:1258  */
        break;

    case 205: /* startup  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2391 "parser.c" /* yacc.c:1258  */
        break;

    case 206: /* textfile  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2397 "parser.c" /* yacc.c:1258  */
        break;

    case 207: /* tooltype  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2403 "parser.c" /* yacc.c:1258  */
        break;

    case 208: /* transcript  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2409 "parser.c" /* yacc.c:1258  */
        break;

    case 209: /* rename  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2415 "parser.c" /* yacc.c:1258  */
        break;

    case 210: /* complete  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2421 "parser.c" /* yacc.c:1258  */
        break;

    case 211: /* debug  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2427 "parser.c" /* yacc.c:1258  */
        break;

    case 212: /* message  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2433 "parser.c" /* yacc.c:1258  */
        break;

    case 213: /* user  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2439 "parser.c" /* yacc.c:1258  */
        break;

    case 214: /* welcome  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2445 "parser.c" /* yacc.c:1258  */
        break;

    case 215: /* working  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2451 "parser.c" /* yacc.c:1258  */
        break;

    case 216: /* database  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2457 "parser.c" /* yacc.c:1258  */
        break;

    case 217: /* earlier  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2463 "parser.c" /* yacc.c:1258  */
        break;

    case 218: /* getassign  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2469 "parser.c" /* yacc.c:1258  */
        break;

    case 219: /* getdevice  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2475 "parser.c" /* yacc.c:1258  */
        break;

    case 220: /* getdiskspace  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2481 "parser.c" /* yacc.c:1258  */
        break;

    case 221: /* getenv  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2487 "parser.c" /* yacc.c:1258  */
        break;

    case 222: /* getsize  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2493 "parser.c" /* yacc.c:1258  */
        break;

    case 223: /* getsum  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2499 "parser.c" /* yacc.c:1258  */
        break;

    case 224: /* getversion  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2505 "parser.c" /* yacc.c:1258  */
        break;

    case 225: /* iconinfo  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2511 "parser.c" /* yacc.c:1258  */
        break;

    case 226: /* dcl  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2517 "parser.c" /* yacc.c:1258  */
        break;

    case 227: /* cus  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2523 "parser.c" /* yacc.c:1258  */
        break;

    case 228: /* askbool  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2529 "parser.c" /* yacc.c:1258  */
        break;

    case 229: /* askchoice  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2535 "parser.c" /* yacc.c:1258  */
        break;

    case 230: /* askdir  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2541 "parser.c" /* yacc.c:1258  */
        break;

    case 231: /* askdisk  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2547 "parser.c" /* yacc.c:1258  */
        break;

    case 232: /* askfile  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2553 "parser.c" /* yacc.c:1258  */
        break;

    case 233: /* asknumber  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2559 "parser.c" /* yacc.c:1258  */
        break;

    case 234: /* askoptions  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2565 "parser.c" /* yacc.c:1258  */
        break;

    case 235: /* askstring  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2571 "parser.c" /* yacc.c:1258  */
        break;

    case 236: /* cat  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2577 "parser.c" /* yacc.c:1258  */
        break;

    case 237: /* expandpath  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2583 "parser.c" /* yacc.c:1258  */
        break;

    case 238: /* fmt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2589 "parser.c" /* yacc.c:1258  */
        break;

    case 239: /* pathonly  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2595 "parser.c" /* yacc.c:1258  */
        break;

    case 240: /* patmatch  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2601 "parser.c" /* yacc.c:1258  */
        break;

    case 241: /* strlen  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2607 "parser.c" /* yacc.c:1258  */
        break;

    case 242: /* substr  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2613 "parser.c" /* yacc.c:1258  */
        break;

    case 243: /* tackon  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2619 "parser.c" /* yacc.c:1258  */
        break;

    case 244: /* set  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2625 "parser.c" /* yacc.c:1258  */
        break;

    case 245: /* symbolset  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2631 "parser.c" /* yacc.c:1258  */
        break;

    case 246: /* symbolval  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2637 "parser.c" /* yacc.c:1258  */
        break;

    case 247: /* openwbobject  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2643 "parser.c" /* yacc.c:1258  */
        break;

    case 248: /* showwbobject  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2649 "parser.c" /* yacc.c:1258  */
        break;

    case 249: /* closewbobject  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2655 "parser.c" /* yacc.c:1258  */
        break;

    case 250: /* all  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2661 "parser.c" /* yacc.c:1258  */
        break;

    case 251: /* append  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2667 "parser.c" /* yacc.c:1258  */
        break;

    case 252: /* assigns  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2673 "parser.c" /* yacc.c:1258  */
        break;

    case 253: /* back  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2679 "parser.c" /* yacc.c:1258  */
        break;

    case 254: /* choices  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2685 "parser.c" /* yacc.c:1258  */
        break;

    case 255: /* command  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2691 "parser.c" /* yacc.c:1258  */
        break;

    case 256: /* compression  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2697 "parser.c" /* yacc.c:1258  */
        break;

    case 257: /* confirm  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2703 "parser.c" /* yacc.c:1258  */
        break;

    case 258: /* default  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2709 "parser.c" /* yacc.c:1258  */
        break;

    case 259: /* delopts  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2715 "parser.c" /* yacc.c:1258  */
        break;

    case 260: /* dest  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2721 "parser.c" /* yacc.c:1258  */
        break;

    case 261: /* disk  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2727 "parser.c" /* yacc.c:1258  */
        break;

    case 262: /* files  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2733 "parser.c" /* yacc.c:1258  */
        break;

    case 263: /* fonts  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2739 "parser.c" /* yacc.c:1258  */
        break;

    case 264: /* getdefaulttool  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2745 "parser.c" /* yacc.c:1258  */
        break;

    case 265: /* getposition  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2751 "parser.c" /* yacc.c:1258  */
        break;

    case 266: /* getstack  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2757 "parser.c" /* yacc.c:1258  */
        break;

    case 267: /* gettooltype  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2763 "parser.c" /* yacc.c:1258  */
        break;

    case 268: /* help  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2769 "parser.c" /* yacc.c:1258  */
        break;

    case 269: /* infos  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2775 "parser.c" /* yacc.c:1258  */
        break;

    case 270: /* include  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2781 "parser.c" /* yacc.c:1258  */
        break;

    case 271: /* newname  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2787 "parser.c" /* yacc.c:1258  */
        break;

    case 272: /* newpath  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2793 "parser.c" /* yacc.c:1258  */
        break;

    case 273: /* nogauge  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2799 "parser.c" /* yacc.c:1258  */
        break;

    case 274: /* noposition  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2805 "parser.c" /* yacc.c:1258  */
        break;

    case 275: /* noreq  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2811 "parser.c" /* yacc.c:1258  */
        break;

    case 276: /* pattern  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2817 "parser.c" /* yacc.c:1258  */
        break;

    case 277: /* prompt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2823 "parser.c" /* yacc.c:1258  */
        break;

    case 278: /* quiet  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2829 "parser.c" /* yacc.c:1258  */
        break;

    case 279: /* range  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2835 "parser.c" /* yacc.c:1258  */
        break;

    case 280: /* safe  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2841 "parser.c" /* yacc.c:1258  */
        break;

    case 281: /* setdefaulttool  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2847 "parser.c" /* yacc.c:1258  */
        break;

    case 282: /* setposition  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2853 "parser.c" /* yacc.c:1258  */
        break;

    case 283: /* setstack  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2859 "parser.c" /* yacc.c:1258  */
        break;

    case 284: /* settooltype  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2865 "parser.c" /* yacc.c:1258  */
        break;

    case 285: /* source  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2871 "parser.c" /* yacc.c:1258  */
        break;

    case 286: /* swapcolors  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2877 "parser.c" /* yacc.c:1258  */
        break;

    case 287: /* optional  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2883 "parser.c" /* yacc.c:1258  */
        break;

    case 288: /* resident  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2889 "parser.c" /* yacc.c:1258  */
        break;

    case 289: /* override  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2895 "parser.c" /* yacc.c:1258  */
        break;

    case 290: /* dynopt  */
#line 80 "../src/parser.y" /* yacc.c:1258  */
      { kill(((*yyvaluep).e)); }
#line 2901 "parser.c" /* yacc.c:1258  */
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
#line 96 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = init((yyvsp[0].e)); }
#line 3169 "parser.c" /* yacc.c:1651  */
    break;

  case 6:
#line 102 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3175 "parser.c" /* yacc.c:1651  */
    break;

  case 7:
#line 103 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3181 "parser.c" /* yacc.c:1651  */
    break;

  case 8:
#line 104 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3187 "parser.c" /* yacc.c:1651  */
    break;

  case 9:
#line 105 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push((yyvsp[-2].e), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3193 "parser.c" /* yacc.c:1651  */
    break;

  case 12:
#line 108 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3199 "parser.c" /* yacc.c:1651  */
    break;

  case 13:
#line 109 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = merge((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3205 "parser.c" /* yacc.c:1651  */
    break;

  case 14:
#line 110 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3211 "parser.c" /* yacc.c:1651  */
    break;

  case 15:
#line 111 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3217 "parser.c" /* yacc.c:1651  */
    break;

  case 16:
#line 112 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = merge((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3223 "parser.c" /* yacc.c:1651  */
    break;

  case 17:
#line 113 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3229 "parser.c" /* yacc.c:1651  */
    break;

  case 18:
#line 114 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3235 "parser.c" /* yacc.c:1651  */
    break;

  case 19:
#line 115 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = (yyvsp[-1].e); }
#line 3241 "parser.c" /* yacc.c:1651  */
    break;

  case 20:
#line 116 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-2].e), (yyvsp[-1].e)); }
#line 3247 "parser.c" /* yacc.c:1651  */
    break;

  case 21:
#line 117 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3253 "parser.c" /* yacc.c:1651  */
    break;

  case 22:
#line 118 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), (yyvsp[0].e)); }
#line 3259 "parser.c" /* yacc.c:1651  */
    break;

  case 23:
#line 119 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), (yyvsp[0].e)); }
#line 3265 "parser.c" /* yacc.c:1651  */
    break;

  case 24:
#line 120 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3271 "parser.c" /* yacc.c:1651  */
    break;

  case 25:
#line 121 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3277 "parser.c" /* yacc.c:1651  */
    break;

  case 26:
#line 122 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_number((yyvsp[0].n)); }
#line 3283 "parser.c" /* yacc.c:1651  */
    break;

  case 27:
#line 123 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_string((yyvsp[0].s)); }
#line 3289 "parser.c" /* yacc.c:1651  */
    break;

  case 28:
#line 124 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_symref((yyvsp[0].s), LINE); }
#line 3295 "parser.c" /* yacc.c:1651  */
    break;

  case 29:
#line 125 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push((yyvsp[-2].e), new_symbol((yyvsp[-1].s))), (yyvsp[0].e)) ; }
#line 3301 "parser.c" /* yacc.c:1651  */
    break;

  case 30:
#line 126 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), new_symbol((yyvsp[-1].s))), (yyvsp[0].e)); }
#line 3307 "parser.c" /* yacc.c:1651  */
    break;

  case 31:
#line 127 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push((yyvsp[-1].e), new_symbol((yyvsp[0].s))); }
#line 3313 "parser.c" /* yacc.c:1651  */
    break;

  case 32:
#line 128 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(new_contxt(), new_symbol((yyvsp[0].s))); }
#line 3319 "parser.c" /* yacc.c:1651  */
    break;

  case 33:
#line 129 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3325 "parser.c" /* yacc.c:1651  */
    break;

  case 34:
#line 130 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = push(push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), (yyvsp[0].e)); }
#line 3331 "parser.c" /* yacc.c:1651  */
    break;

  case 165:
#line 267 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("+"), LINE, m_add, (yyvsp[-1].e), NUMBER); }
#line 3337 "parser.c" /* yacc.c:1651  */
    break;

  case 166:
#line 268 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("/"), LINE, m_div, (yyvsp[-1].e), NUMBER); }
#line 3343 "parser.c" /* yacc.c:1651  */
    break;

  case 167:
#line 269 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("*"), LINE, m_mul, (yyvsp[-1].e), NUMBER); }
#line 3349 "parser.c" /* yacc.c:1651  */
    break;

  case 168:
#line 270 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("-"), LINE, m_sub, (yyvsp[-1].e), NUMBER); }
#line 3355 "parser.c" /* yacc.c:1651  */
    break;

  case 169:
#line 273 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("AND"), LINE, m_and, (yyvsp[-1].e), NUMBER); }
#line 3361 "parser.c" /* yacc.c:1651  */
    break;

  case 170:
#line 274 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITAND"), LINE, m_bitand, (yyvsp[-1].e), NUMBER); }
#line 3367 "parser.c" /* yacc.c:1651  */
    break;

  case 171:
#line 275 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITNOT"), LINE, m_bitnot, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3373 "parser.c" /* yacc.c:1651  */
    break;

  case 172:
#line 276 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITOR"), LINE, m_bitor, (yyvsp[-1].e), NUMBER); }
#line 3379 "parser.c" /* yacc.c:1651  */
    break;

  case 173:
#line 277 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("BITXOR"), LINE, m_bitxor, (yyvsp[-1].e), NUMBER); }
#line 3385 "parser.c" /* yacc.c:1651  */
    break;

  case 174:
#line 278 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("NOT"), LINE, m_not, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3391 "parser.c" /* yacc.c:1651  */
    break;

  case 175:
#line 279 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("IN"), LINE, m_in, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3397 "parser.c" /* yacc.c:1651  */
    break;

  case 176:
#line 280 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("OR"), LINE, m_or, (yyvsp[-1].e), NUMBER); }
#line 3403 "parser.c" /* yacc.c:1651  */
    break;

  case 177:
#line 281 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("shiftleft"), LINE, m_shiftleft, (yyvsp[-1].e), NUMBER); }
#line 3409 "parser.c" /* yacc.c:1651  */
    break;

  case 178:
#line 282 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("shiftright"), LINE, m_shiftright, (yyvsp[-1].e), NUMBER); }
#line 3415 "parser.c" /* yacc.c:1651  */
    break;

  case 179:
#line 283 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("XOR"), LINE, m_xor, (yyvsp[-1].e), NUMBER); }
#line 3421 "parser.c" /* yacc.c:1651  */
    break;

  case 180:
#line 286 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("="), LINE, m_eq, (yyvsp[-1].e), NUMBER); }
#line 3427 "parser.c" /* yacc.c:1651  */
    break;

  case 181:
#line 287 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup(">"), LINE, m_gt, (yyvsp[-1].e), NUMBER); }
#line 3433 "parser.c" /* yacc.c:1651  */
    break;

  case 182:
#line 288 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup(">="), LINE, m_gte, (yyvsp[-1].e), NUMBER); }
#line 3439 "parser.c" /* yacc.c:1651  */
    break;

  case 183:
#line 289 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<"), LINE, m_lt, (yyvsp[-1].e), NUMBER); }
#line 3445 "parser.c" /* yacc.c:1651  */
    break;

  case 184:
#line 290 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<="), LINE, m_lte, (yyvsp[-1].e), NUMBER); }
#line 3451 "parser.c" /* yacc.c:1651  */
    break;

  case 185:
#line 291 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("<>"), LINE, m_neq, (yyvsp[-1].e), NUMBER); }
#line 3457 "parser.c" /* yacc.c:1651  */
    break;

  case 186:
#line 294 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, (yyvsp[-1].e), NUMBER); }
#line 3463 "parser.c" /* yacc.c:1651  */
    break;

  case 187:
#line 295 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, (yyvsp[-1].e), NUMBER); }
#line 3469 "parser.c" /* yacc.c:1651  */
    break;

  case 188:
#line 296 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("if"), LINE, m_if, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3475 "parser.c" /* yacc.c:1651  */
    break;

  case 189:
#line 297 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("select"), LINE, m_select, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3481 "parser.c" /* yacc.c:1651  */
    break;

  case 190:
#line 298 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("until"), LINE, m_until, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3487 "parser.c" /* yacc.c:1651  */
    break;

  case 191:
#line 299 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("while"), LINE, m_while, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3493 "parser.c" /* yacc.c:1651  */
    break;

  case 192:
#line 300 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("trace"), LINE, m_trace, NULL, NUMBER); }
#line 3499 "parser.c" /* yacc.c:1651  */
    break;

  case 193:
#line 301 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("retrace"), LINE, m_retrace, NULL, DANGLE); }
#line 3505 "parser.c" /* yacc.c:1651  */
    break;

  case 194:
#line 304 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3511 "parser.c" /* yacc.c:1651  */
    break;

  case 195:
#line 305 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3517 "parser.c" /* yacc.c:1651  */
    break;

  case 196:
#line 306 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3523 "parser.c" /* yacc.c:1651  */
    break;

  case 197:
#line 307 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("execute"), LINE, m_execute, (yyvsp[-1].e), NUMBER); }
#line 3529 "parser.c" /* yacc.c:1651  */
    break;

  case 198:
#line 308 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3535 "parser.c" /* yacc.c:1651  */
    break;

  case 199:
#line 309 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3541 "parser.c" /* yacc.c:1651  */
    break;

  case 200:
#line 310 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3547 "parser.c" /* yacc.c:1651  */
    break;

  case 201:
#line 311 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rexx"), LINE, m_rexx, (yyvsp[-1].e), NUMBER); }
#line 3553 "parser.c" /* yacc.c:1651  */
    break;

  case 202:
#line 312 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3559 "parser.c" /* yacc.c:1651  */
    break;

  case 203:
#line 313 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3565 "parser.c" /* yacc.c:1651  */
    break;

  case 204:
#line 314 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, push((yyvsp[-2].e), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3571 "parser.c" /* yacc.c:1651  */
    break;

  case 205:
#line 315 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("run"), LINE, m_run, (yyvsp[-1].e), NUMBER); }
#line 3577 "parser.c" /* yacc.c:1651  */
    break;

  case 206:
#line 318 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("abort"), LINE, m_abort, (yyvsp[-1].e), NUMBER); }
#line 3583 "parser.c" /* yacc.c:1651  */
    break;

  case 207:
#line 319 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3589 "parser.c" /* yacc.c:1651  */
    break;

  case 208:
#line 320 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push((yyvsp[-1].e), (yyvsp[-2].e)), NUMBER); }
#line 3595 "parser.c" /* yacc.c:1651  */
    break;

  case 209:
#line 321 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3601 "parser.c" /* yacc.c:1651  */
    break;

  case 210:
#line 322 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, (yyvsp[-1].e), NUMBER); }
#line 3607 "parser.c" /* yacc.c:1651  */
    break;

  case 211:
#line 323 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exit"), LINE, m_exit, NULL, NUMBER); }
#line 3613 "parser.c" /* yacc.c:1651  */
    break;

  case 212:
#line 324 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("onerror"), LINE, m_procedure, push(new_contxt(),
                                                        new_custom(strdup("@onerror"), LINE, NULL, (yyvsp[-1].e))), DANGLE); }
#line 3620 "parser.c" /* yacc.c:1651  */
    break;

  case 213:
#line 326 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("reboot"), LINE, m_reboot, NULL, NUMBER); }
#line 3626 "parser.c" /* yacc.c:1651  */
    break;

  case 214:
#line 327 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("trap"), LINE, m_trap, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3632 "parser.c" /* yacc.c:1651  */
    break;

  case 215:
#line 330 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("copyfiles"), LINE, m_copyfiles, (yyvsp[-1].e), NUMBER); }
#line 3638 "parser.c" /* yacc.c:1651  */
    break;

  case 216:
#line 331 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("copylib"), LINE, m_copylib, (yyvsp[-1].e), NUMBER); }
#line 3644 "parser.c" /* yacc.c:1651  */
    break;

  case 217:
#line 332 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3650 "parser.c" /* yacc.c:1651  */
    break;

  case 218:
#line 333 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3656 "parser.c" /* yacc.c:1651  */
    break;

  case 219:
#line 334 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3662 "parser.c" /* yacc.c:1651  */
    break;

  case 220:
#line 335 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("delete"), LINE, m_delete, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3668 "parser.c" /* yacc.c:1651  */
    break;

  case 221:
#line 336 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exists"), LINE, m_exists, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3674 "parser.c" /* yacc.c:1651  */
    break;

  case 222:
#line 337 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("exists"), LINE, m_exists, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3680 "parser.c" /* yacc.c:1651  */
    break;

  case 223:
#line 338 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("fileonly"), LINE, m_fileonly, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3686 "parser.c" /* yacc.c:1651  */
    break;

  case 224:
#line 339 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("foreach"), LINE, m_foreach, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3692 "parser.c" /* yacc.c:1651  */
    break;

  case 225:
#line 340 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3698 "parser.c" /* yacc.c:1651  */
    break;

  case 226:
#line 341 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, (yyvsp[-1].e), NUMBER); }
#line 3704 "parser.c" /* yacc.c:1651  */
    break;

  case 227:
#line 342 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3710 "parser.c" /* yacc.c:1651  */
    break;

  case 228:
#line 343 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makeassign"), LINE, m_makeassign, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3716 "parser.c" /* yacc.c:1651  */
    break;

  case 229:
#line 344 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3722 "parser.c" /* yacc.c:1651  */
    break;

  case 230:
#line 345 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3728 "parser.c" /* yacc.c:1651  */
    break;

  case 231:
#line 346 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3734 "parser.c" /* yacc.c:1651  */
    break;

  case 232:
#line 347 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("makedir"), LINE, m_makedir, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3740 "parser.c" /* yacc.c:1651  */
    break;

  case 233:
#line 348 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3746 "parser.c" /* yacc.c:1651  */
    break;

  case 234:
#line 349 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, (yyvsp[-1].e), NUMBER); }
#line 3752 "parser.c" /* yacc.c:1651  */
    break;

  case 235:
#line 350 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3758 "parser.c" /* yacc.c:1651  */
    break;

  case 236:
#line 351 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("protect"), LINE, m_protect, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3764 "parser.c" /* yacc.c:1651  */
    break;

  case 237:
#line 352 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3770 "parser.c" /* yacc.c:1651  */
    break;

  case 238:
#line 353 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 3776 "parser.c" /* yacc.c:1651  */
    break;

  case 239:
#line 354 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 3782 "parser.c" /* yacc.c:1651  */
    break;

  case 240:
#line 355 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("startup"), LINE, m_startup, push(push(new_contxt(), new_symref(strdup("@app-name"), LINE)), (yyvsp[-1].e)), NUMBER); }
#line 3788 "parser.c" /* yacc.c:1651  */
    break;

  case 241:
#line 356 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("textfile"), LINE, m_textfile, (yyvsp[-1].e), NUMBER); }
#line 3794 "parser.c" /* yacc.c:1651  */
    break;

  case 242:
#line 357 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("tooltype"), LINE, m_tooltype, (yyvsp[-1].e), NUMBER); }
#line 3800 "parser.c" /* yacc.c:1651  */
    break;

  case 243:
#line 358 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("transcript"), LINE, m_transcript, (yyvsp[-1].e), NUMBER); }
#line 3806 "parser.c" /* yacc.c:1651  */
    break;

  case 244:
#line 359 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rename"), LINE, m_rename, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3812 "parser.c" /* yacc.c:1651  */
    break;

  case 245:
#line 360 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("rename"), LINE, m_rename, (yyvsp[-1].e), NUMBER); }
#line 3818 "parser.c" /* yacc.c:1651  */
    break;

  case 246:
#line 363 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("complete"), LINE, m_complete, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3824 "parser.c" /* yacc.c:1651  */
    break;

  case 247:
#line 364 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("debug"), LINE, m_debug, (yyvsp[-1].e), NUMBER); }
#line 3830 "parser.c" /* yacc.c:1651  */
    break;

  case 248:
#line 365 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("debug"), LINE, m_debug, NULL, NUMBER); }
#line 3836 "parser.c" /* yacc.c:1651  */
    break;

  case 249:
#line 366 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, push((yyvsp[-2].e), (yyvsp[-1].e)), NUMBER); }
#line 3842 "parser.c" /* yacc.c:1651  */
    break;

  case 250:
#line 367 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("message"), LINE, m_message, (yyvsp[-1].e), NUMBER); }
#line 3848 "parser.c" /* yacc.c:1651  */
    break;

  case 251:
#line 368 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("user"), LINE, m_user, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3854 "parser.c" /* yacc.c:1651  */
    break;

  case 252:
#line 369 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("welcome"), LINE, m_welcome, (yyvsp[-1].e), NUMBER); }
#line 3860 "parser.c" /* yacc.c:1651  */
    break;

  case 253:
#line 370 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("welcome"), LINE, m_welcome, NULL, NUMBER); }
#line 3866 "parser.c" /* yacc.c:1651  */
    break;

  case 254:
#line 371 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("working"), LINE, m_working, (yyvsp[-1].e), NUMBER); }
#line 3872 "parser.c" /* yacc.c:1651  */
    break;

  case 255:
#line 374 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("database"), LINE, m_database, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3878 "parser.c" /* yacc.c:1651  */
    break;

  case 256:
#line 375 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("database"), LINE, m_database, (yyvsp[-1].e), STRING); }
#line 3884 "parser.c" /* yacc.c:1651  */
    break;

  case 257:
#line 376 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("earlier"), LINE, m_earlier, (yyvsp[-1].e), NUMBER); }
#line 3890 "parser.c" /* yacc.c:1651  */
    break;

  case 258:
#line 377 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getassign"), LINE, m_getassign, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3896 "parser.c" /* yacc.c:1651  */
    break;

  case 259:
#line 378 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getassign"), LINE, m_getassign, (yyvsp[-1].e), STRING); }
#line 3902 "parser.c" /* yacc.c:1651  */
    break;

  case 260:
#line 379 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdevice"), LINE, m_getdevice, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3908 "parser.c" /* yacc.c:1651  */
    break;

  case 261:
#line 380 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdiskspace"), LINE, m_getdiskspace, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3914 "parser.c" /* yacc.c:1651  */
    break;

  case 262:
#line 381 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getdiskspace"), LINE, m_getdiskspace, (yyvsp[-1].e), NUMBER); }
#line 3920 "parser.c" /* yacc.c:1651  */
    break;

  case 263:
#line 382 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getenv"), LINE, m_getenv, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 3926 "parser.c" /* yacc.c:1651  */
    break;

  case 264:
#line 383 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getsize"), LINE, m_getsize, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3932 "parser.c" /* yacc.c:1651  */
    break;

  case 265:
#line 384 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getsum"), LINE, m_getsum, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3938 "parser.c" /* yacc.c:1651  */
    break;

  case 266:
#line 385 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, NULL, NUMBER); }
#line 3944 "parser.c" /* yacc.c:1651  */
    break;

  case 267:
#line 386 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 3950 "parser.c" /* yacc.c:1651  */
    break;

  case 268:
#line 387 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("getversion"), LINE, m_getversion, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 3956 "parser.c" /* yacc.c:1651  */
    break;

  case 269:
#line 388 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("iconinfo"), LINE, m_iconinfo, (yyvsp[-1].e), NUMBER); }
#line 3962 "parser.c" /* yacc.c:1651  */
    break;

  case 270:
#line 391 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-3].s), LINE, (yyvsp[-2].e), (yyvsp[-1].e))), NUMBER); }
#line 3968 "parser.c" /* yacc.c:1651  */
    break;

  case 271:
#line 392 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-2].s), LINE, (yyvsp[-1].e), NULL)), NUMBER); }
#line 3974 "parser.c" /* yacc.c:1651  */
    break;

  case 272:
#line 393 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-2].s), LINE, NULL, (yyvsp[-1].e))), NUMBER); }
#line 3980 "parser.c" /* yacc.c:1651  */
    break;

  case 273:
#line 394 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("procedure"), LINE, m_procedure, push(new_contxt(), new_custom((yyvsp[-1].s), LINE, NULL, NULL)), NUMBER); }
#line 3986 "parser.c" /* yacc.c:1651  */
    break;

  case 274:
#line 395 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_cusref((yyvsp[-2].s), LINE, (yyvsp[-1].e)); }
#line 3992 "parser.c" /* yacc.c:1651  */
    break;

  case 275:
#line 396 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_cusref((yyvsp[-1].s), LINE, NULL); }
#line 3998 "parser.c" /* yacc.c:1651  */
    break;

  case 276:
#line 399 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askbool"), LINE, m_askbool, NULL, NUMBER); }
#line 4004 "parser.c" /* yacc.c:1651  */
    break;

  case 277:
#line 400 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askbool"), LINE, m_askbool, (yyvsp[-1].e), NUMBER); }
#line 4010 "parser.c" /* yacc.c:1651  */
    break;

  case 278:
#line 401 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askchoice"), LINE, m_askchoice, (yyvsp[-1].e), NUMBER); }
#line 4016 "parser.c" /* yacc.c:1651  */
    break;

  case 279:
#line 402 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdir"), LINE, m_askdir, NULL, STRING); }
#line 4022 "parser.c" /* yacc.c:1651  */
    break;

  case 280:
#line 403 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdir"), LINE, m_askdir, (yyvsp[-1].e), STRING); }
#line 4028 "parser.c" /* yacc.c:1651  */
    break;

  case 281:
#line 404 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askdisk"), LINE, m_askdisk, (yyvsp[-1].e), NUMBER); }
#line 4034 "parser.c" /* yacc.c:1651  */
    break;

  case 282:
#line 405 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askfile"), LINE, m_askfile, NULL, STRING); }
#line 4040 "parser.c" /* yacc.c:1651  */
    break;

  case 283:
#line 406 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askfile"), LINE, m_askfile, (yyvsp[-1].e), STRING); }
#line 4046 "parser.c" /* yacc.c:1651  */
    break;

  case 284:
#line 407 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("asknumber"), LINE, m_asknumber, NULL, NUMBER); }
#line 4052 "parser.c" /* yacc.c:1651  */
    break;

  case 285:
#line 408 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("asknumber"), LINE, m_asknumber, (yyvsp[-1].e), NUMBER); }
#line 4058 "parser.c" /* yacc.c:1651  */
    break;

  case 286:
#line 409 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askoptions"), LINE, m_askoptions, (yyvsp[-1].e), NUMBER); }
#line 4064 "parser.c" /* yacc.c:1651  */
    break;

  case 287:
#line 410 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askstring"), LINE, m_askstring, NULL, STRING); }
#line 4070 "parser.c" /* yacc.c:1651  */
    break;

  case 288:
#line 411 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("askstring"), LINE, m_askstring, (yyvsp[-1].e), STRING); }
#line 4076 "parser.c" /* yacc.c:1651  */
    break;

  case 289:
#line 414 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("cat"), LINE, m_cat, (yyvsp[-1].e), STRING); }
#line 4082 "parser.c" /* yacc.c:1651  */
    break;

  case 290:
#line 415 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("expandpath"), LINE, m_expandpath, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 4088 "parser.c" /* yacc.c:1651  */
    break;

  case 291:
#line 416 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native((yyvsp[-2].s), LINE, m_fmt, (yyvsp[-1].e), STRING); }
#line 4094 "parser.c" /* yacc.c:1651  */
    break;

  case 292:
#line 417 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native((yyvsp[-1].s), LINE, m_fmt, NULL, STRING); }
#line 4100 "parser.c" /* yacc.c:1651  */
    break;

  case 293:
#line 418 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("pathonly"), LINE, m_pathonly, push(new_contxt(), (yyvsp[-1].e)), STRING); }
#line 4106 "parser.c" /* yacc.c:1651  */
    break;

  case 294:
#line 419 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("patmatch"), LINE, m_patmatch, (yyvsp[-1].e), NUMBER); }
#line 4112 "parser.c" /* yacc.c:1651  */
    break;

  case 295:
#line 420 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("strlen"), LINE, m_strlen, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4118 "parser.c" /* yacc.c:1651  */
    break;

  case 296:
#line 421 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("substr"), LINE, m_substr, (yyvsp[-1].e), STRING); }
#line 4124 "parser.c" /* yacc.c:1651  */
    break;

  case 297:
#line 422 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("substr"), LINE, m_substr, push((yyvsp[-2].e), (yyvsp[-1].e)), STRING); }
#line 4130 "parser.c" /* yacc.c:1651  */
    break;

  case 298:
#line 423 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("tackon"), LINE, m_tackon, (yyvsp[-1].e), STRING); }
#line 4136 "parser.c" /* yacc.c:1651  */
    break;

  case 299:
#line 426 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("set"), LINE, m_set, (yyvsp[-1].e), DANGLE); }
#line 4142 "parser.c" /* yacc.c:1651  */
    break;

  case 300:
#line 427 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("symbolset"), LINE, m_symbolset, (yyvsp[-1].e), DANGLE); }
#line 4148 "parser.c" /* yacc.c:1651  */
    break;

  case 301:
#line 428 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("symbolval"), LINE, m_symbolval, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4154 "parser.c" /* yacc.c:1651  */
    break;

  case 302:
#line 431 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4160 "parser.c" /* yacc.c:1651  */
    break;

  case 303:
#line 432 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-2].e)), (yyvsp[-1].e)), NUMBER); }
#line 4166 "parser.c" /* yacc.c:1651  */
    break;

  case 304:
#line 433 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-1].e)), (yyvsp[-2].e)), NUMBER); }
#line 4172 "parser.c" /* yacc.c:1651  */
    break;

  case 305:
#line 434 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("openwbobject"), LINE, m_openwbobject, push(push(new_contxt(), (yyvsp[-2].e)), merge((yyvsp[-3].e), (yyvsp[-1].e))), NUMBER); }
#line 4178 "parser.c" /* yacc.c:1651  */
    break;

  case 306:
#line 435 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("showwbobject"), LINE, m_showwbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4184 "parser.c" /* yacc.c:1651  */
    break;

  case 307:
#line 436 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_native(strdup("closewbobject"), LINE, m_closewbobject, push(new_contxt(), (yyvsp[-1].e)), NUMBER); }
#line 4190 "parser.c" /* yacc.c:1651  */
    break;

  case 308:
#line 439 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("all"), OPT_ALL, NULL); }
#line 4196 "parser.c" /* yacc.c:1651  */
    break;

  case 309:
#line 440 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("append"), OPT_APPEND, (yyvsp[-1].e)); }
#line 4202 "parser.c" /* yacc.c:1651  */
    break;

  case 310:
#line 441 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("assigns"), OPT_ASSIGNS, NULL); }
#line 4208 "parser.c" /* yacc.c:1651  */
    break;

  case 311:
#line 442 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("back"), OPT_BACK, (yyvsp[-1].e)); }
#line 4214 "parser.c" /* yacc.c:1651  */
    break;

  case 312:
#line 443 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("choices"), OPT_CHOICES, (yyvsp[-1].e)); }
#line 4220 "parser.c" /* yacc.c:1651  */
    break;

  case 313:
#line 444 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("command"), OPT_COMMAND, (yyvsp[-1].e)); }
#line 4226 "parser.c" /* yacc.c:1651  */
    break;

  case 314:
#line 445 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("compression"), OPT_COMPRESSION, NULL); }
#line 4232 "parser.c" /* yacc.c:1651  */
    break;

  case 315:
#line 446 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("confirm"), OPT_CONFIRM, push(new_contxt(), (yyvsp[-1].e))); }
#line 4238 "parser.c" /* yacc.c:1651  */
    break;

  case 316:
#line 447 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("confirm"), OPT_CONFIRM, NULL); }
#line 4244 "parser.c" /* yacc.c:1651  */
    break;

  case 317:
#line 448 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("default"), OPT_DEFAULT, push(new_contxt(), (yyvsp[-1].e))); }
#line 4250 "parser.c" /* yacc.c:1651  */
    break;

  case 318:
#line 449 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("delopts"), OPT_DELOPTS, (yyvsp[-1].e)); }
#line 4256 "parser.c" /* yacc.c:1651  */
    break;

  case 319:
#line 450 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("dest"), OPT_DEST, push(new_contxt(), (yyvsp[-1].e))); }
#line 4262 "parser.c" /* yacc.c:1651  */
    break;

  case 320:
#line 451 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("disk"), OPT_DISK, NULL); }
#line 4268 "parser.c" /* yacc.c:1651  */
    break;

  case 321:
#line 452 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("files"), OPT_FILES, NULL); }
#line 4274 "parser.c" /* yacc.c:1651  */
    break;

  case 322:
#line 453 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("fonts"), OPT_FONTS, NULL); }
#line 4280 "parser.c" /* yacc.c:1651  */
    break;

  case 323:
#line 454 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getdefaulttool"), OPT_GETDEFAULTTOOL, push(new_contxt(), (yyvsp[-1].e))); }
#line 4286 "parser.c" /* yacc.c:1651  */
    break;

  case 324:
#line 455 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getposition"), OPT_GETPOSITION, (yyvsp[-1].e)); }
#line 4292 "parser.c" /* yacc.c:1651  */
    break;

  case 325:
#line 456 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("getstack"), OPT_GETSTACK, push(new_contxt(), (yyvsp[-1].e))); }
#line 4298 "parser.c" /* yacc.c:1651  */
    break;

  case 326:
#line 457 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("gettooltype"), OPT_GETTOOLTYPE, (yyvsp[-1].e)); }
#line 4304 "parser.c" /* yacc.c:1651  */
    break;

  case 327:
#line 458 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("help"), OPT_HELP, (yyvsp[-1].e)); }
#line 4310 "parser.c" /* yacc.c:1651  */
    break;

  case 328:
#line 459 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("help"), OPT_HELP, push(new_contxt(), new_symref(strdup("@null"), LINE))); }
#line 4316 "parser.c" /* yacc.c:1651  */
    break;

  case 329:
#line 460 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("infos"), OPT_INFOS, NULL); }
#line 4322 "parser.c" /* yacc.c:1651  */
    break;

  case 330:
#line 461 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("include"), OPT_INCLUDE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4328 "parser.c" /* yacc.c:1651  */
    break;

  case 331:
#line 462 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("newname"), OPT_NEWNAME, push(new_contxt(), (yyvsp[-1].e))); }
#line 4334 "parser.c" /* yacc.c:1651  */
    break;

  case 332:
#line 463 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("newpath"), OPT_NEWPATH, NULL); }
#line 4340 "parser.c" /* yacc.c:1651  */
    break;

  case 333:
#line 464 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("nogauge"), OPT_NOGAUGE, NULL); }
#line 4346 "parser.c" /* yacc.c:1651  */
    break;

  case 334:
#line 465 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("noposition"), OPT_NOPOSITION, NULL); }
#line 4352 "parser.c" /* yacc.c:1651  */
    break;

  case 335:
#line 466 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("noreq"), OPT_NOREQ, NULL); }
#line 4358 "parser.c" /* yacc.c:1651  */
    break;

  case 336:
#line 467 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("pattern"), OPT_PATTERN, push(new_contxt(), (yyvsp[-1].e))); }
#line 4364 "parser.c" /* yacc.c:1651  */
    break;

  case 337:
#line 468 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("prompt"), OPT_PROMPT, (yyvsp[-1].e)); }
#line 4370 "parser.c" /* yacc.c:1651  */
    break;

  case 338:
#line 469 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("quiet"), OPT_QUIET, NULL); }
#line 4376 "parser.c" /* yacc.c:1651  */
    break;

  case 339:
#line 470 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("range"), OPT_RANGE, (yyvsp[-1].e)); }
#line 4382 "parser.c" /* yacc.c:1651  */
    break;

  case 340:
#line 471 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("safe"), OPT_SAFE, NULL); }
#line 4388 "parser.c" /* yacc.c:1651  */
    break;

  case 341:
#line 472 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setdefaulttool"), OPT_SETDEFAULTTOOL, push(new_contxt(), (yyvsp[-1].e))); }
#line 4394 "parser.c" /* yacc.c:1651  */
    break;

  case 342:
#line 473 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setposition"), OPT_SETPOSITION, (yyvsp[-1].e)); }
#line 4400 "parser.c" /* yacc.c:1651  */
    break;

  case 343:
#line 474 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("setstack"), OPT_SETSTACK, push(new_contxt(), (yyvsp[-1].e))); }
#line 4406 "parser.c" /* yacc.c:1651  */
    break;

  case 344:
#line 475 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("settooltype"), OPT_SETTOOLTYPE, (yyvsp[-1].e)); }
#line 4412 "parser.c" /* yacc.c:1651  */
    break;

  case 345:
#line 476 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("settooltype"), OPT_SETTOOLTYPE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4418 "parser.c" /* yacc.c:1651  */
    break;

  case 346:
#line 477 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("source"), OPT_SOURCE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4424 "parser.c" /* yacc.c:1651  */
    break;

  case 347:
#line 478 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("swapcolors"), OPT_SWAPCOLORS, NULL); }
#line 4430 "parser.c" /* yacc.c:1651  */
    break;

  case 348:
#line 479 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("optional"), OPT_OPTIONAL, (yyvsp[-1].e)); }
#line 4436 "parser.c" /* yacc.c:1651  */
    break;

  case 349:
#line 480 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("resident"), OPT_RESIDENT, NULL); }
#line 4442 "parser.c" /* yacc.c:1651  */
    break;

  case 350:
#line 481 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("override"), OPT_OVERRIDE, push(new_contxt(), (yyvsp[-1].e))); }
#line 4448 "parser.c" /* yacc.c:1651  */
    break;

  case 351:
#line 482 "../src/parser.y" /* yacc.c:1651  */
    { (yyval.e) = new_option(strdup("dynopt"), OPT_DYNOPT, push(push(push(new_contxt(), (yyvsp[-3].e)), (yyvsp[-2].e)), (yyvsp[-1].e))); }
#line 4454 "parser.c" /* yacc.c:1651  */
    break;


#line 4458 "parser.c" /* yacc.c:1651  */
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
#line 484 "../src/parser.y" /* yacc.c:1910  */


