/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * $Id$
 */

#define X_NULL          1
#define X_PROGRAM       2
#define X_STATS         3
#define X_COMMAND       4
#define X_ADDR_V        5
#define X_ADDR_N        6
#define X_ADDR_S        7
#define X_CALL          8
#define X_DO            9
#define X_REP           10
#define X_REP_FOREVER   11
#define X_REP_COUNT     12
#define X_DO_TO         13
#define X_DO_BY         14
#define X_DO_FOR        15
#define X_WHILE         16
#define X_UNTIL         17
#define X_DROP          18
#define X_EXIT          19
#define X_IF            20
#define X_IPRET         21
#define X_ITERATE       22
#define X_LABEL         23
#define X_LEAVE         24
#define X_NUM_D         25
#define X_NUM_F         26
#define X_NUM_FUZZ      27
#define X_NUM_SCI       28
#define X_NUM_ENG       29
#define X_PARSE         30
#define X_PARSE_U       31
#define X_PARSE_ARG     32
#define X_PARSE_EXT     33
#define X_PARSE_NUM     34
#define X_PARSE_PULL    35
#define X_PARSE_SRC     36
#define X_PARSE_VAR     37
#define X_PARSE_VAL     38
#define X_PARSE_VER     39
#define X_PARSE_ARG_U   40
#define X_PROC          41
#define X_PULL          42
#define X_PUSH          43
#define X_QUEUE         44
#define X_RETURN        45
#define X_SAY           46
#define X_SELECT        47
#define X_WHENS         48
#define X_WHEN          49
#define X_OTHERWISE     50
#define X_SIG_VAL       51
#define X_SIG_LAB       52
#define X_SIG_SET       53
#define X_ON            54
#define X_OFF           55
#define X_S_ERROR       56
#define X_S_HALT        57
#define X_S_NOVALUE     58
#define X_S_SYNTAX      59
#define X_TRACE         60
#define X_T_ALL         61
#define X_T_COMM        62
#define X_T_ERR         63
#define X_T_INTER       64
#define X_T_LABEL       65
#define X_T_NORMAL      66
#define X_T_OFF         67
#define X_T_RESULT      68
#define X_T_SCAN        69
#define X_WHAT_BB       70
#define X_WHAT_BW       71
#define X_WHAT_B        72
#define X_WHAT_WB       73
#define X_WHAT_WW       74
#define X_WHAT_W        75
#define X_UPPER_VAR     76
#define X_ASSIGN        77
#define X_LOG_NOT       78
#define X_PLUSS         79
#define X_EQUAL         80
#define X_MINUS         81
#define X_MULT          82
#define X_DEVIDE        83
#define X_MODULUS       84
#define X_LOG_OR        85
#define X_LOG_AND       86
#define X_LOG_XOR       87
#define X_EXP           88
#define X_CONCAT        89
#define X_SPACE         90
#define X_GTE           91
#define X_LTE           92
#define X_GT            93
#define X_LT            94
#define X_DIFF          95
#define X_SIM_SYMBOL    96
#define X_CON_SYMBOL    97
#define X_HEX_STR       98
#define X_STRING        99
#define X_FUNC          100
#define X_U_MINUS       101
#define X_S_EQUAL       102
#define X_S_DIFF        103
#define X_SIMSYMB       104
#define X_INTDIV        105
#define X_EX_FUNC       106
#define X_IN_FUNC       107
#define X_TPL_SOLID     108
#define X_TPL_MVE       109
#define X_TPL_VAR       110
#define X_TPL_TO        111
#define X_TPL_SYMBOL    112
#define X_TPL_SPACE     113
#define X_TPL_POINT     114
#define X_TMPLS         115
#define X_TPL_OFF       116
#define X_TPL_PATT      117
#define X_POS_OFFS      118
#define X_NEG_OFFS      119
#define X_ABS_OFFS      120
#define X_EXPRLIST      121
#define X_SYMBOLS       122
#define X_SYMBOL        123
#define X_ARG           124
#define X_S_NOTREADY    125
#define X_S_FAILURE     126
#define X_END           127
#define X_CALL_SET      128
#define X_IN_CALL       129
#define X_NO_OTHERWISE  130
#define X_IND_SYMBOL    131
#define X_IS_INTERNAL   132
#define X_IS_BUILTIN    133
#define X_IS_EXTERNAL   134
#define X_CTAIL_SYMBOL  135
#define X_VTAIL_SYMBOL  136
#define X_HEAD_SYMBOL   137
#define X_STEM_SYMBOL   138
#define X_SEQUAL        139
#define X_SDIFF         140
#define X_SGT           141
#define X_SGTE          142
#define X_SLT           143
#define X_SLTE          144
#define X_NEQUAL        145
#define X_NDIFF         146
#define X_NGT           147
#define X_NGTE          148
#define X_NLT           149
#define X_NLTE          150
#define X_NASSIGN       151
#define X_CEXPRLIST     152
#define X_U_PLUSS       153
#define X_TPL_NEG       154
#define X_TPL_POS       155
#define X_TPL_ABS       156
#define X_OPTIONS       157
#define X_NUM_V         158
#define X_NUM_DDEF      159
#define X_NUM_FDEF      160
#define X_NUM_FRMDEF    161
#define X_SNGT          162
#define X_SNLT          163
#define X_S_NGT         164
#define X_S_NLT         166
#define X_S_GT          167
#define X_S_GTE         168
#define X_S_LT          169
#define X_S_LTE         170
#define X_ADDR_W        171


/* The three first two numbers have not errortext attched to them */
#define ERR_PROG_UNREADABLE     3
#define ERR_PROG_INTERRUPT      4
#define ERR_STORAGE_EXHAUSTED   5
#define ERR_UNMATCHED_QUOTE     6
#define ERR_WHEN_EXPECTED       7
#define ERR_THEN_UNEXPECTED     8
#define ERR_WHEN_UNEXPECTED     9
#define ERR_UNMATCHED_END       10
#define ERR_FULL_CTRL_STACK     11
#define ERR_TOO_LONG_LINE       12
#define ERR_INVALID_CHAR        13
#define ERR_INCOMPLETE_STRUCT   14
#define ERR_INVALID_HEX_CONST   15
#define ERR_UNEXISTENT_LABEL    16
#define ERR_UNEXPECTED_PROC     17
#define ERR_THEN_EXPECTED       18
#define ERR_STRING_EXPECTED     19
#define ERR_SYMBOL_EXPECTED     20
#define ERR_EXTRA_DATA          21
/* The next to number have not been assigned an errortext */
#define ERR_INVALID_TRACE       24
#define ERR_INV_SUBKEYWORD      25
#define ERR_INVALID_INTEGER     26
#define ERR_INVALID_DO_SYNTAX   27
#define ERR_INVALID_LEAVE       28
#define ERR_ENVIRON_TOO_LONG    29
#define ERR_TOO_LONG_STRING     30
#define ERR_INVALID_START       31
#define ERR_INVALID_STEM        32
#define ERR_INVALID_RESULT      33
#define ERR_UNLOGICAL_VALUE     34
#define ERR_INVALID_EXPRESSION  35
#define ERR_UNMATCHED_PARAN     36
#define ERR_UNEXPECTED_PARAN    37
#define ERR_INVALID_TEMPLATE    38
#define ERR_STACK_OVERFLOW      39
#define ERR_INCORRECT_CALL      40
#define ERR_BAD_ARITHMETIC      41
#define ERR_ARITH_OVERFLOW      42
#define ERR_ROUTINE_NOT_FOUND   43
#define ERR_NO_DATA_RETURNED    44
#define ERR_DATA_NOT_SPEC       45
/* No errortexts have not been defined to the next to numbers */
#define ERR_SYSTEM_FAILURE      48
#define ERR_INTERPRETER_FAILURE 49

#define ERR_CANT_REWIND         60
#define ERR_IMPROPER_SEEK       61
#if 0
#define ERR_BUFFER_TOO_SMALL    62
#define ERR_NO_SUCH_FILE        63
#endif
#define ERR_YACC_SYNTAX         64

#define ERR_UNKNOWN_FS_ERROR    90
#define ERR_STREAM_COMMAND      93

#define ERR_EXTERNAL_QUEUE      94

#define ERR_STORAGE_EXHAUSTED_TMPL       "System resources exhausted"

#define ERR_RXSTACK_CANT_CONNECT       101
#define ERR_RXSTACK_CANT_CONNECT_TMPL    "Error connecting to %s on port %d: \"%s\""
#define ERR_RXSTACK_NO_IP              102
#define ERR_RXSTACK_NO_IP_TMPL           "Unable to obtain IP address for %s"
#define ERR_RXSTACK_INVALID_SERVER     103
#define ERR_RXSTACK_INVALID_SERVER_TMPL  "Invalid format for server in specified queue name: \"%s\""
#define ERR_RXSTACK_INVALID_QUEUE      104
#define ERR_RXSTACK_INVALID_QUEUE_TMPL   "Invalid format for queue name: \"%s\""
#define ERR_RXSTACK_NO_WINSOCK         105
#define ERR_RXSTACK_NO_WINSOCK_TMPL      "Unable to start Windows Socket interface: %s"
#define ERR_RXSTACK_TOO_MANY_QUEUES    106
#define ERR_RXSTACK_TOO_MANY_QUEUES_TMPL "Maximum number of external queues exceeded: %d"
#define ERR_RXSTACK_READING_SOCKET     107
#define ERR_RXSTACK_READING_SOCKET_TMPL  "Error occured reading socket: %s"
#define ERR_RXSTACK_INVALID_SWITCH     108
#define ERR_RXSTACK_INVALID_SWITCH_TMPL  "Invalid switch passed. Must be one of \"%s\""

#define ERR_RXSTACK_INTERNAL            99
#define ERR_RXSTACK_INTERNAL_TMPL        "Internal error with external queue interface: %d \"%s\""
#define ERR_RXSTACK_GENERAL            100
#define ERR_RXSTACK_GENERAL_TMPL         "General system error with external queue interface. %s. %s"

/*
 #define ENV_BOURNE_SH  0
 #define ENV_C_SHELL    1
 #define ENV_COMMAND     2
 #define ENV_PATH        3
 #define ENV_SYSTEM      4
 */

#ifdef TRACEMEM
/*
 * NOTE: There is a close correspondance between these and the char
 *       array alloc in memory.c
 */
# define TRC_LEAKED     0
# define TRC_HASHTAB    1
# define TRC_PROCBOX    2
# define TRC_SOURCE     3
# define TRC_SOURCEL    4
# define TRC_TREENODE   5
# define TRC_VARVALUE   6
# define TRC_VARNAME    7
# define TRC_VARBOX     8
# define TRC_STACKBOX   9
# define TRC_STACKLINE 10
# define TRC_SYSINFO   11
# define TRC_FILEPTR   12
# define TRC_PROCARG   13
# define TRC_LABEL     14
# define TRC_STATIC    15
# define TRC_P_CACHE   16
# define TRC_MATH      17
# define TRC_ENVIRBOX  18
# define TRC_ENVIRNAME 19
# define TRC_SPCV_BOX  20
# define TRC_SPCV_NAME 21
# define TRC_SPCV_NUMB 22
# define TRC_SPCV_NUMC 23



# define MEMTRC_NONE    0
# define MEMTRC_ALL     1
# define MEMTRC_LEAKED  2

# define MEM_ALLOC      0
# define MEM_CURRENT    1
# define MEM_LEAKED     2
#endif /* TRACEMEM */


/*
 * Note: must match the definition of  numeric_forms  in rexx.c
 */
#define NUM_FORM_SCI    0
#define NUM_FORM_ENG    1


#define SIGNAL_FATAL   -1
#define SIGNAL_ERROR    0
#define SIGNAL_FAILURE  1
#define SIGNAL_HALT     2
#define SIGNAL_NOVALUE  3
#define SIGNAL_NOTREADY 4
#define SIGNAL_SYNTAX   5
#define SIGNALS         6

#define SIGTYPE_ON    0
#define SIGTYPE_OFF   1
#define SIGTYPE_DELAY 2


#define ENVIR_SHELL 1
#define ENVIR_PIPE 2
#if defined(_AMIGA) || defined(__AROS__)
#define ENVIR_AMIGA 3
#endif

#define SUBENVIR_PATH 1
#define SUBENVIR_SYSTEM 2
#define SUBENVIR_COMMAND 3


#define REDIR_NONE     0x00000000
#define REDIR_INPUT    0x00000001
#define REDIR_OUTLIFO  0x00000002
#define REDIR_OUTFIFO  0x00000004
#define REDIR_OUTPUT   0x00000006
#define REDIR_CLEAR    0x00000008


#define HOOK_LINEFEED  1
#define HOOK_NOLF      0

#define HOOK_GO_ON     1
#define HOOK_NOPE      0

#define HOOK_STDOUT    0
#define HOOK_STDERR    1
#define HOOK_TRCIN     2
#define HOOK_PULL      3
#define HOOK_INIT      4
#define HOOK_TERMIN    5
#define HOOK_SUBCOM    6
#define HOOK_FUNC      7
#define HOOK_GETENV    8
#define HOOK_SETENV    9

#define HOOK_MASK(a)   (1<<(a))

#define PARAM_TYPE_HARD 0
#define PARAM_TYPE_SOFT 1

#define INVO_COMMAND    0
#define INVO_FUNCTION   1
#define INVO_SUBROUTINE 2

#define SYMBOL_BAD      0
#define SYMBOL_CONSTANT 1
#define SYMBOL_STEM     2
#define SYMBOL_SIMPLE   3
#define SYMBOL_COMPOUND 4


#define VFLAG_NONE   0x0000
#define VFLAG_STR    0x0001
#define VFLAG_NUM    0x0002

#define VFLAG_BOTH   (VFLAG_STR | VFLAG_NUM)

#define UNKNOWN_QUEUE -1
/*
 * For refering to the extensions, must correspond with the bitfields
 * in the definition of the u.options field of proclevel in types.h
 */
#define EXT_FLUSHSTACK             0
#define EXT_LINEOUTTRUNC           1
#define EXT_CLOSE_BIF              2
#define EXT_OPEN_BIF               3
#define EXT_BUFTYPE_BIF            4
#define EXT_DESBUF_BIF             5
#define EXT_DROPBUF_BIF            6
#define EXT_MAKEBUF_BIF            7
#define EXT_CACHEEXT               8
#define EXT_FIND_BIF               9
#define EXT_PRUNE_TRACE           10
#define EXT_EXT_COMMANDS_AS_FUNCS 11
#define EXT_STDOUT_FOR_STDERR     12
#define EXT_TRACE_HTML            13
#define EXT_FAST_LINES_BIF_DEFAULT 14
#define EXT_ANSI                  15
#define EXT_INTERNAL_QUEUES       16
#define EXT_PGB_PATCH1            17  /* pgb */
