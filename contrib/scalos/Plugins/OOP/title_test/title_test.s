; title_test.s
; 02 Jan 2004 20:22:13

;---------------------------------------------------------------
	machine	68020

	incdir	"SC:Assembler_Headers"

	NOLIST		; skip includes in listing

	include	"exec/types.i"
	include	"exec/macros.i"
	include	"exec/resident.i"
	include	"exec/initializers.i"
	include	"scalos/iconobject.i"
	include	"scalos/scalos.i"
	include	"scalos_macros.i"
	include "lvo/scalos_lib.i"
	include	"asmsupp.i"

;---------------------------------------------------------------

Startlib:
    moveq   #0,d0
    rts

; This is so that the library doesn't crash if you try to run
; it as normal executable

;---------------------------------------------------------------


RomTag:
	dc.w	RTC_MATCHWORD	;RT_MATCHWORD
	dc.l	RomTag		;RT_MATCHTAG
	dc.l	endep		;RT_ENDSKIP
	dc.b	RTF_AUTOINIT	;RT_FLAGS
	dc.b	39		;RT_VERSION
	dc.b	9		;RT_TYPE
	dc.b	0		;RT_PRI
	dc.l	libname		;RT_NAME
	dc.l	idstring	;RT_IDSTRING
	dc.l	init		;RT_INIT

	cnop	0,4

;---------------------------------------------------------------

; AmigaOS detects a library or device by the above ROMTag structure.
;
; From <exec/resident.i>:
;
; STRUCTURE RT,0
;    UWORD RT_MATCHWORD          ; word to match on (ILLEGAL)
;    APTR  RT_MATCHTAG           ; pointer to the above (RT_MATCHWORD)
;    APTR  RT_ENDSKIP            ; address to continue scan
;    UBYTE RT_FLAGS              ; various tag flags
;    UBYTE RT_VERSION            ; release version number
;    UBYTE RT_TYPE               ; type of module (NT_XXXXXX)
;    BYTE  RT_PRI                ; initialization priority
;    APTR  RT_NAME               ; pointer to node name
;    APTR  RT_IDSTRING           ; pointer to identification string
;    APTR  RT_INIT               ; pointer to init code
;    LABEL RT_SIZE
;
; RTC_MATCHWORD   EQU $4AFC       ; The 68000 "ILLEGAL" instruction
;
; The system uses LoadSeg() to get the library code into memory and then
; does an InitResident() call on it.
;
; This scans through the code until it finds the ROMTag structure. It is
; recognized by RTC_MATCHWORD, which is an illegal 68000 opcode.
;
; If the RTF_AUTOINIT flags is set in RT_FLAGS then the address in RT_INIT
; is a pointer to a table of longs used to initialize the library.
;
;        - The size of your library/device base structure including initial
;          Library or Device structure.
;
;        - A pointer to a longword table of standard, then library
;          specific function offsets, terminated with -1L.
;          (short format offsets are also acceptable)
;
;        - Pointer to data table in exec/InitStruct format for
;          initialization of Library or Device astructure.
;
;        - Pointer to library initialization function, or NULL.
;        Calling sequence:
;            D0 = library base
;            A0 = segList
;            A6 = ExecBase
;          This function must return in D0 the library/device base to be
;          linked into the library/device list.  If the initialization
;          function fails, the device memory must be manually deallocated,
;          then NULL returned in D0.


libname:
    dc.b    'title_freepens.plugin',0
    cnop    0,4
idstring:
    dc.b    '$VER: title_freepens.plugin 39.1 (05-09-97)',10
    dc.b    '(c)1997 by ALiENDESiGN',0
    cnop    0,4

;---------------------------------------------------------------

init:
    dc.l    38               ; Size of library base
    dc.l    functable        ; Pointer to function table
    dc.l    datatable        ; Pointer to data init table
    dc.l    InitRoutine      ; Pointer to init function

;---------------------------------------------------------------

; Table of offsets for library functions

functable:
    dc.l    Open            ; Called when a client opens the library
    dc.l    Close           ; Called when a client close the library
    dc.l    Expunge         ; Called when the library needs to be flushed from memory
    dc.l    Startlib        ; Currently unused.


    dc.l    getclassinfo    ; This is where you start your library functions

    dc.l    -1              ; Marks the end of the table


;---------------------------------------------------------------

; This the data table used to fill various values in the
; the library base vector.
; This in a format understood by the InitStruct() function


datatable:
    INITBYTE    8,9             ;LN_TYPE,NT_LIBRARY
    INITLONG    10,libname      ;LN_NAME,LibName
    INITBYTE    14,3            ;LIB_FLAGS,LIBF_SUMUSED!LIBF_CHANGED
    INITWORD    20,39           ;LIB_VERSION,VERSION
    INITWORD    22,1            ;LIB_REVISION,REVISION
    INITLONG    24,idstring     ;LIB_IDSTRING,IDString
    dc.l    0

    cnop    0,4

;---------------------------------------------------------------

; Init routine - called when library is loaded into memory.
; Parameters
;     D0 = library base
;     A0 = segList          (This needs to be stored so we can manually
;                            unload the library from memory)
;     A6 = ExecBase

InitRoutine:
    movem.l d0-a6,-(sp)

    move.l  a0,seglist

    move.l  d0,a0
    move.l  #'PLUG',34(a0)      ;Store identifier for plugins into LibBase

    movem.l (sp)+,d0-a6
    rts
.openerror:
    movem.l (sp)+,d0-a6
    moveq   #0,d0
    rts

;---------------------------------------------------------------

Open:                   ; ( libptr:a6, version:d0 )
    addq.w  #1,32(a6)       ; LIB_OPENCNT
    bclr    #3,14(a6)       ; LIB_FLAGS
    move.l  a6,d0
    rts

;---------------------------------------------------------------

Close:                  ; ( libptr:a6 )
    subq.w  #1,32(a6)       ; LIB_OPENCNT
    bne.s   .nooneopen
    btst    #3,14(a6)       ; LIB_FLAGS
    beq.s   .nooneopen
    bsr.s   Expunge
.nooneopen:
    moveq   #0,d0
    rts

;---------------------------------------------------------------

Expunge:                ; ( libptr: a6 )
    tst.w   32(a6)
    beq.s   .nooneopen2
    bset    #3,14(a6)
    moveq   #0,d0
    rts

.nooneopen2:
    movem.l d2/a4/a5/a6,-(sp)
    move.l  seglist(pc),d2
    move.l  a6,a4
    move.l  4.w,a6
    move.l  a4,a1
    jsr -252(a6)        ;Remove
    moveq   #0,d0
    move.l  a4,a1
    move.w  16(a4),d0       ;LIB_NEGSIZE
    sub.w   d0,a1
    add.w   18(a4),d0       ;LIB_POSSIZE
    jsr -210(a6)        ;FreeMem

    move.l  d2,d0
    movem.l (sp)+,d2/a4/a5/a6
    rts

;----------------------------- GetClass Info --------------------------------

;Input: none
;Ouput: ClassInfo
getclassinfo:
    lea classinfo(pc),a0
    move.l  a0,d0
    rts

classinfo:
    dc.l    0,0
    dc.l    hookfunc
    dc.l    0,0
    dc.w    0           ;Priority
    dc.w    0           ;Instance Size
    dc.w    39          ;Needed Scalos Version
    dc.w    0           ;Reserved
    dc.l    superclassname      ;Name of the Class
    dc.l    superclassname      ;Name of the SuperClass
    dc.l    name
    dc.l    description
    dc.l    makername

superclassname: dc.b    'Title.sca',0
name:       dc.b    'Title FreePens',0
description:    dc.b    'Adds a %wp function to the screentitle to show all free pens.',0
makername:  dc.b    'Stefan Sommerfeld',0
    even

;-------------------------------- Class -------------------------------------

hookfunc:
    cmp.l   #SCCM_Title_Translate,(a1)
    beq.s   .translate
.call_super:
    move.l  24(a0),a0       ;Call Superclass
    move.l  8(a0),-(a7)
    rts

.translate:
    cmp.w   #'wp',12(a1)
    bne.s   .call_super
    move.l  a3,-(a7)
    move.l  4(a1),a3        ;TextPtr
    moveq   #0,d0
    move.l  4(a2),a0        ;Internal Info
    move.l  12(a0),a0       ;Screen
    cmp.w   #0,a0
    beq.s   .noscreen
    move.l  48(a0),a0       ;ColorMap
    move.l  40(a0),a0       ;PaletteExtra
    moveq   #0,d0
    move.w  48(a0),d0       ;FreePens
.noscreen:
    bsr.s   hextoascii
    move.l  a3,4(a1)        ;set the new offset
    move.l  (a7)+,a3
    moveq   #1,d0
    rts


;--------------------------------------------
;Hexvalue to decimal ascii
;Input: D0 = Value - 32Bit
;Output: Text to A3
hextoascii:
    movem.l d0-d1/a2,-(a7)
    move.l  a7,a2
    lea -14(a7),a7
    clr.b   -(a2)
.loop:  moveq   #0,d1

    swap    d0      ;4
    move.w  d0,d1       ;4  ;HiNibble => d1
    beq.s   .nohinib    ;8  ;HiNibble =0 => Div Only Lo!
    divu    #10,d1      ;144    ;HiNibble/10
    move.w  d1,d0       ;   ;Result =>d0
.nohinib:
    swap    d0      ;4  ;Restore LowNibble
    move.w  d0,d1       ;4  ;Transfer for Div
    divu    #10,d1      ;144    ;LowNibble/10
    move.w  d1,d0       ;4  ;Result => d0
    swap    d1      ;4  ;Result!

    add.b   #$30,d1
    move.b  d1,-(a2)
    tst.l   d0
    bne.s   .loop
.exit:  move.b  (a2)+,(a3)+
    tst.b   (a2)
    bne.s   .exit
    lea 14(a7),a7
    movem.l (a7)+,d0-d1/a2
    rts



;----------------------------------------------------------------------------

seglist:    dc.l    0
endep:

