/********************************************
*  INPUT.H  08/04/90
*  © Copyright 1990 Timm Martin
*  This source code is freely distributable
*  and may be used without compensation in
*  any commercial or non-commercial product
*  as long as this notice is included and
*  remains intact.  This code may be used
*  in an executable program without
*  acknowledgement of the author.
*********************************************/

#ifndef INPUT_H
#define INPUT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

/*--- Qualifiers ---*/
#define QUAL_COMMAND 0x0100
#define QUAL_LSHIFT 0x0200
#define QUAL_RSHIFT 0x0400
#define QUAL_SHIFT (QUAL_LSHIFT|QUAL_RSHIFT)
#define QUAL_CONTROL 0x0800
#define QUAL_LALT 0x1000
#define QUAL_RALT 0x2000
#define QUAL_ALT (QUAL_LALT|QUAL_RALT)
#define QUAL_LAMIGA 0x4000
#define QUAL_RAMIGA 0x8000
#define QUAL_AMIGA (QUAL_LAMIGA|QUAL_RAMIGA)
#define QUAL_QUAL (QUAL_COMMAND|QUAL_SHIFT|QUAL_CONTROL|QUAL_ALT|QUAL_AMIGA)

/*--- Keys ---*/
#define KEY_VALUE(k) ((k)&0x00FF)
#define KEY_NORMAL(k) (!((k)&(QUAL_CONTROL|QUAL_ALT|QUAL_AMIGA)))
#define KEY_QUALIFIERS(k) ((k)&QUAL_QUAL)
#define KEY_COMMAND(k) ((k)&QUAL_COMMAND)
#define KEY_SHIFT(k) ((k)&QUAL_SHIFT)
#define KEY_CONTROL(k) ((k)&QUAL_CONTROL)
#define KEY_ALT(k) ((k)&QUAL_ALT)
#define KEY_CHAR(k) (!((k)&(QUAL_COMMAND|QUAL_CONTROL|QUAL_AMIGA)))
#define KEY_AMIGA(k) ((k)&QUAL_AMIGA)

/*--- Command Keys ---*/
#define COMMAND_F1 1
#define COMMAND_F2 2
#define COMMAND_F3 3
#define COMMAND_F4 4
#define COMMAND_F5 5
#define COMMAND_F6 6
#define COMMAND_F7 7
#define COMMAND_F8 8
#define COMMAND_F9 9
#define COMMAND_F10 10
#define COMMAND_TAB 11
#define COMMAND_DELETE 12
#define COMMAND_ESCAPE 13
#define COMMAND_BACKSPACE 14
#define COMMAND_HELP 15
#define COMMAND_RETURN 16
#define COMMAND_UP_ARROW 17
#define COMMAND_DOWN_ARROW 18
#define COMMAND_LEFT_ARROW 19
#define COMMAND_RIGHT_ARROW 20

/*--- Functions ---*/
extern void console_close( void );
extern BOOL console_open( void );
extern USHORT input_key( struct IntuiMessage *imessage );

#endif
