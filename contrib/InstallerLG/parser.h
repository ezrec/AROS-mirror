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
    CLOSEMEDIA = 315,
    EFFECT = 316,
    SETMEDIA = 317,
    SHOWMEDIA = 318,
    DATABASE = 319,
    EARLIER = 320,
    GETASSIGN = 321,
    GETDEVICE = 322,
    GETDISKSPACE = 323,
    GETENV = 324,
    GETSIZE = 325,
    GETSUM = 326,
    GETVERSION = 327,
    ICONINFO = 328,
    CUS = 329,
    DCL = 330,
    ASKBOOL = 331,
    ASKCHOICE = 332,
    ASKDIR = 333,
    ASKDISK = 334,
    ASKFILE = 335,
    ASKNUMBER = 336,
    ASKOPTIONS = 337,
    ASKSTRING = 338,
    CAT = 339,
    EXPANDPATH = 340,
    FMT = 341,
    PATHONLY = 342,
    PATMATCH = 343,
    STRLEN = 344,
    SUBSTR = 345,
    TACKON = 346,
    SET = 347,
    SYMBOLSET = 348,
    SYMBOLVAL = 349,
    OPENWBOBJECT = 350,
    SHOWWBOBJECT = 351,
    CLOSEWBOBJECT = 352,
    ALL = 353,
    APPEND = 354,
    ASSIGNS = 355,
    BACK = 356,
    CHOICES = 357,
    COMMAND = 358,
    COMPRESSION = 359,
    CONFIRM = 360,
    DEFAULT = 361,
    DELOPTS = 362,
    DEST = 363,
    DISK = 364,
    FILES = 365,
    FONTS = 366,
    GETDEFAULTTOOL = 367,
    GETPOSITION = 368,
    GETSTACK = 369,
    GETTOOLTYPE = 370,
    HELP = 371,
    INFOS = 372,
    INCLUDE = 373,
    NEWNAME = 374,
    NEWPATH = 375,
    NOGAUGE = 376,
    NOPOSITION = 377,
    NOREQ = 378,
    PATTERN = 379,
    PROMPT = 380,
    QUIET = 381,
    RANGE = 382,
    SAFE = 383,
    SETDEFAULTTOOL = 384,
    SETPOSITION = 385,
    SETSTACK = 386,
    SETTOOLTYPE = 387,
    SOURCE = 388,
    SWAPCOLORS = 389,
    OPTIONAL = 390,
    RESIDENT = 391,
    OVERRIDE = 392
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
#define CLOSEMEDIA 315
#define EFFECT 316
#define SETMEDIA 317
#define SHOWMEDIA 318
#define DATABASE 319
#define EARLIER 320
#define GETASSIGN 321
#define GETDEVICE 322
#define GETDISKSPACE 323
#define GETENV 324
#define GETSIZE 325
#define GETSUM 326
#define GETVERSION 327
#define ICONINFO 328
#define CUS 329
#define DCL 330
#define ASKBOOL 331
#define ASKCHOICE 332
#define ASKDIR 333
#define ASKDISK 334
#define ASKFILE 335
#define ASKNUMBER 336
#define ASKOPTIONS 337
#define ASKSTRING 338
#define CAT 339
#define EXPANDPATH 340
#define FMT 341
#define PATHONLY 342
#define PATMATCH 343
#define STRLEN 344
#define SUBSTR 345
#define TACKON 346
#define SET 347
#define SYMBOLSET 348
#define SYMBOLVAL 349
#define OPENWBOBJECT 350
#define SHOWWBOBJECT 351
#define CLOSEWBOBJECT 352
#define ALL 353
#define APPEND 354
#define ASSIGNS 355
#define BACK 356
#define CHOICES 357
#define COMMAND 358
#define COMPRESSION 359
#define CONFIRM 360
#define DEFAULT 361
#define DELOPTS 362
#define DEST 363
#define DISK 364
#define FILES 365
#define FONTS 366
#define GETDEFAULTTOOL 367
#define GETPOSITION 368
#define GETSTACK 369
#define GETTOOLTYPE 370
#define HELP 371
#define INFOS 372
#define INCLUDE 373
#define NEWNAME 374
#define NEWPATH 375
#define NOGAUGE 376
#define NOPOSITION 377
#define NOREQ 378
#define PATTERN 379
#define PROMPT 380
#define QUIET 381
#define RANGE 382
#define SAFE 383
#define SETDEFAULTTOOL 384
#define SETPOSITION 385
#define SETSTACK 386
#define SETTOOLTYPE 387
#define SOURCE 388
#define SWAPCOLORS 389
#define OPTIONAL 390
#define RESIDENT 391
#define OVERRIDE 392

/* Value type.  */



int yyparse (yyscan_t scanner);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
