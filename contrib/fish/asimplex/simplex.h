/*****************************************************************************
 * Variablendeklaration : simplex.h                                          *
 * Autor                : Stefan Förster                                     *
 *                                                                           *
 * Datum      | Version | Bemerkung                                          *
 * -----------|---------|--------------------------------------------------- *
 * 06.02.1989 | 0.0     |                                                    *
 * 26.02.1989 | 0.1     | EPS_NULL                                           *
 * 26.02.1989 | 0.2     | VERBOSE, INVERT_FREQUENCY                          *
 * 05.03.1989 | 0.3     | #include <errno.h>                                 *
 * 06.03.1989 | 0.4     | PERCENT auf 0.03 geändert                          *
 * 08.03.1989 | 0.5     | Definitionen für mpsx.c                            *
 * 14.03.1989 | 1.0     | #define ERR_...                                    *
 *****************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
//#include <stat.h>
#include <math.h>
#include <exec/exec.h>
#include <exec/tasks.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

#define INT               int


#define NOT_OPTIMAL       (USHORT)0x0001
#define OPTIMAL           (USHORT)0x0002
#define NOT_UNBOUNDED     (USHORT)0x0004
#define UNBOUNDED         (USHORT)0x0008
#define CLEAR_CUT         (USHORT)0x0010
#define EMPTY             (USHORT)0x0020
#define INVERTABLE        (USHORT)0x0040
#define NOT_INVERTABLE    (USHORT)0x0080
#define PHASE_I           (USHORT)0x0100
#define PHASE_II          (USHORT)0x0200
#define SMALLEST_INDEX    (USHORT)0x0400
#define STEEPEST_ASCENT   (USHORT)0x0800
#define LAMBDA_0          (USHORT)0x1000
#define LAMBDA_1          (USHORT)0x2000
#define LAMBDA_2          (USHORT)0x4000
#define VERBOSE           (USHORT)0x8000

#define _TRUE             1
#define _FALSE            0

#define PERCENT           0.03    /* 3% : Kriterium, welche PRICE-     */
                                  /* methode angewendet wird           */
#define INVERT_FREQUENCY  50L

#define EPS_INV           1e-15   /* Kriterium, daß Matrix singulär    */
#define EPS_NULL          1e-13   /* == 0 */

#define ABS(x)            ((SHORT)( (x)<0 ? -(x) : (x) ))
#define SGN(x)            ((SHORT)( (x)<0 ? -1 : 1))  /* SGN(0)==1 */

#define S_DOUBLE          ((LONG)sizeof(DOUBLE))
#define S_SHORT           ((LONG)sizeof(SHORT))


#define MAX_STRLEN        8       /* Namen dürfen max. 8 Zeichen lang sein */
#define BUFFER            256
#define BUFFER2           80
#define MAX_FILELEN       80      /* willkürlich */

#define NAME              0
#define ROWS              1
#define GOAL              ROWS
#define COLUMNS           2
#define RHS               3
#define RANGES            4
#define BOUNDS            5

#define NUM_SYMBOLS       6


#define VAR_LIST          0
#define ROWS_LIST         1
#define GOALS_LIST        2
#define RHS_LIST          3
#define RANGES_LIST       4
#define BOUNDS_LIST       5

#define NUM_LISTS         6


#define BIT_NAME          (USHORT)0x0001
#define BIT_ROWS          (USHORT)0x0002
#define BIT_COLUMNS       (USHORT)0x0004
#define BIT_RHS           (USHORT)0x0008
#define BIT_RANGES        (USHORT)0x0010
#define BIT_BOUNDS        (USHORT)0x0020
#define BIT_ENDATA        (USHORT)0x0040
#define BIT_GOAL          (USHORT)0x0080



typedef struct item {
          TEXT          string[MAX_STRLEN+2];
          SHORT         nr, anz;
          struct item   *next;
        } ITEM, *ITEMPTR;

#define SIZE_ITEM         (LONG)sizeof(ITEM)



#define ERR_INVALID_ARGS    sys_nerr+1
#define ERR_NAME_TOO_LONG   sys_nerr+2
#define ERR_DOUBLE          sys_nerr+3
#define ERR_UNKNOWN_ID      sys_nerr+4
#define ERR_SECTIONS        sys_nerr+5
#define ERR_2SECTIONS       sys_nerr+6
#define ERR_UNKNOWN_SEC     sys_nerr+7
#define ERR_ORDER           sys_nerr+8
#define ERR_NO_NAME         sys_nerr+9
#define ERR_NO_ROWS         sys_nerr+10
#define ERR_NO_GOAL         sys_nerr+11
#define ERR_NO_COLUMNS      sys_nerr+12
#define ERR_NO_RHS          sys_nerr+13
#define ERR_NO_ENDATA       sys_nerr+14
#define ERR_INV_ROWS_TYPE   sys_nerr+15
#define ERR_INV_BOUNDS_TYPE sys_nerr+16
#define ERR_UP_LO           sys_nerr+17
#define ERR_INV_RANGES      sys_nerr+18
#define ERR_MISSING         sys_nerr+19
#define ERR_FILE_TOO_LONG   sys_nerr+20
#define ERR_NOT_READ        sys_nerr+21
#define ERR_NOT_WRITE       sys_nerr+22
#define ERR_EOF             sys_nerr+23
#define ERR_MEM             sys_nerr+24
#define ERR_FATAL           sys_nerr+25
