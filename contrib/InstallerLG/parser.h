/* A Bison parser, made by GNU Bison 3.1.  */

/* Bison interface for Yacc-like parsers in C

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
