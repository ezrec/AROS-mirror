;
; @(#) $Header$
;
; BGUI library
; miscasm.asm
; Miscellaneous assembly stuff.
;
; (C) Copyright 1998 Manuel Lemos.
; (C) Copyright 1996-1997 Ian J. Einman.
; (C) Copyright 1993-1996 Jaba Development.
; (C) Copyright 1993-1996 Jan van den Baard.
; All Rights Reserved.
;
; $Log$
; Revision 41.11  2000/05/09 19:54:47  mlemos
; Merged with the branch Manuel_Lemos_fixes.
;
; Revision 41.10.2.4  1998/12/14 05:04:36  mlemos
; Moved the method dispatcher to a C function in classes.c .
;
; Revision 41.10.2.3  1998/10/01 04:28:50  mlemos
; Made the global class dispatcher function get BGUI global data pointer into
; register a4 and class global data pointer to a5 exchanging registers before
; calling the method function.
;
; Revision 41.10.2.2  1998/09/12 02:18:19  mlemos
; Added Janne patch to call the stack extension code.
;
; Revision 41.10.2.1  1998/02/27 03:32:08  mlemos
; Moved funcdef.i include to exec_lib.i
;
; Revision 41.10  1998/02/25 21:12:42  mlemos
; Bumping to 41.10
;
; Revision 1.1  1998/02/25 17:09:11  mlemos
; Ian sources
;
;
;

         XDEF  _DisplayAGuideInfo
         XDEF  _ScaleWeight
         
         XREF  _UtilityBase,_GfxBase,_IntuitionBase

         XREF  _EnsureStack,_RevertStack

BGUI_STACKEXT EQU 1

         SECTION text,CODE

call     MACRO
         jsr _LVO\1(a6)
         ENDM

         include  "exec/exec_lib.i"
         include  "include:utility/hooks.i"
         include  "include:utility/utility_lib.i"
         include  "include:intuition/classes.i"
         include  "include:intuition/cghooks.i"
         include  "include:intuition/intuition_lib.i"
         include  "include:graphics/graphics_lib.i"
         include  "include:libraries/amigaguide_lib.i"

;makeproto __stdargs BOOL DisplayAGuideInfo(struct NewAmigaGuide *, Tag, ...);

_DisplayAGuideInfo
            movem.l  a2/a6,-(a7)       ; save regs
            move.l   4.w,A6            ; SysBase to a6
            lea      aguideName(pc),a1 ; libname to a1
            moveq    #0,D0             ; any version
            call     OpenLibrary
            tst.l    d0                ; ok?
            beq.s    noSuccess         ; Noop!
            move.l   d0,a6             ; move base to a6
            move.l   12(a7),a0         ; nag to a0
            lea      16(a7),a1         ; attr pointer to a1
            call     OpenAmigaGuideA
            tst.l    d0                ; ok?
            beq.s    noOpen            ; Noop!
            move.l   d0,a0             ; move to a0
            call     CloseAmigaGuide
            bsr.s    LibClose
            moveq    #1,d0             ; TRUE 4 success
            movem.l  (a7)+,a2/a6       ; pop regs
            rts
noOpen      BSR.B LibClose
noSuccess   moveq    #0,d0             ; FALSE 4 failure
            movem.l (sp)+,a2/a6
            rts
LibClose    move.l   a6,a1             ; base to a1
            move.l   4.w,a6            ; SysBase to a6
            call     CloseLibrary
            rts

;makeproto ASM ULONG ScaleWeight(REG(d2) ULONG e, REG(d3) ULONG f, REG(d4) ULONG a);

_ScaleWeight   move.l   a6,-(a7)
               move.l   _UtilityBase(a4),a6
               move.l   d4,d0
               move.l   d2,d1
               call     UMult32
               move.l   d3,d1
               lsr.l    #1,d1
               add.l    d1,d0
               move.l   d3,d1
               call     UDivMod32
               move.l   (a7)+,a6
               rts

         XDEF  _SPrintfA,_StrLenfA
         
;      len = StrLenfA(format, args);
;
;      Returns the length of the formatted string.
;      The length returned is including the terminating
;      0 byte!

;makeproto __stdargs ASM ULONG StrLenfA(REG(a0) UBYTE *, REG(a1) ULONG *);

_StrLenfA   movem.l  a2-a3/a6,-(a7)
            suba.l   a3,a3             ; Counter in a3
            lea      incCnt(pc),a2     ; Counter routine in a2
            move.l   $4.w,a6
            jsr      -522(a6)
            move.l   a3,d0             ; Counter as return code
            movem.l  (a7)+,a2-a3/a6    ; Pop registers
            rts
incCnt      addq.l   #1,a3             ; Increase the counter by one
            rts


;      SPrintfA( buffer, format, args );
;
;      Perform fomatted output into a buffer.

;makeproto __stdargs ASM VOID SPrintfA(REG(a3) UBYTE *, REG(a0) UBYTE *, REG(a1) ULONG *);

_SPrintfA   movem.l  a2-a3/a6,-(a7)
            lea      putChar(pc),a2       ; Byte copy routine in a2
            move.l   $4.w,a6
            jsr      -522(a6)
            movem.l (a7)+,a2-a3/a6        ; Pop registers
            rts
putChar     move.b   d0,(a3)+             ; Put formatted char in buffer
            rts


                    XDEF     _LHook_Format,_LHook_Count

;makeproto ASM VOID LHook_Count(REG(a0) struct Hook *hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc);

_LHook_Count        addq.l   #1,h_Data(a0)
                    rts

;makeproto ASM VOID LHook_Format(REG(a0) struct Hook *hook, REG(a1) ULONG chr, REG(a2) struct Locale *loc);

_LHook_Format       move.l   a1,d0
                    move.l   h_Data(a0),a1
                    move.b   d0,(a1)+
                    move.l   a1,h_Data(a0)
                    rts

;------------------ Method Functions --------------------------------

                    XDEF     _AsmDoMethod,_AsmDoMethodA
                    XDEF     _AsmCoerceMethod,_AsmCoerceMethodA
                    XDEF     _AsmDoSuperMethod,_AsmDoSuperMethodA

;makeproto ULONG AsmDoMethod( Object *, ... );

_AsmDoMethod        move.l   a2,-(a7)
                    lea      8(a7),a1       ; Msg pointer in A1
                    move.l   (a1)+,a2       ; Object to A2
                    bra.s    doMethod       ; Call method

;makeproto ULONG AsmDoSuperMethod( Class *, Object *, ... );

_AsmDoSuperMethod   move.l   a2,-(a7)
                    lea      8(a7),a1       ; Msg pointer in A1
                    movem.l  (a1)+,a0/a2    ; Class in A0, Object to A2
                    bra.s    doSuper

;makeproto ULONG AsmCoerceMethod( Class *, Object *, ... );

_AsmCoerceMethod    move.l   a2,-(a7)
                    lea      8(a7),a1       ; Msg pointer in A1
                    movem.l  (a1)+,a0/a2    ; Class in A0, Object to A2
                    bra.s    doCoerce       ; Call method

;makeproto ASM ULONG AsmDoMethodA( REG(a2) Object *, REG(a1) Msg );

_AsmDoMethodA       move.l   a2,-(a7)
doMethod            move.l   -4(a2),a0      ; Object class to A0
                    bra.s    doCoerce       ; Call method

;makeproto ASM ULONG AsmDoSuperMethodA( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );

_AsmDoSuperMethodA  move.l   a2,-(a7)
doSuper             move.l   24(a0),a0      ; cl_Super to A0
                    bra.s    doCoerce

;makeproto ASM ULONG AsmCoerceMethodA( REG(a0) Class *, REG(a2) Object *, REG(a1) Msg );

_AsmCoerceMethodA   move.l   a2,-(a7)
doCoerce            pea      .rts(pc)       ; Push method return address
                    move.l   8(a0),-(a7)    ; Push class h_Entry
                    rts                     ; Jump to class dispatcher
.rts                move.l   (a7)+,a2       ; Restore A2
                    rts



         XDEF  _BGUI_ObtainGIRPort
         ;
         ;  This routine will prevent
         ;  BGUI to lockup when used
         ;  with hacks like MagicMenu.
         ;

;makeproto ASM struct RastPort *BGUI_ObtainGIRPort( REG(a0) struct GadgetInfo * );

_BGUI_ObtainGIRPort:
            movem.l  a3/a6,-(a7)    ; save a6
            suba.l   a3,a3
            move.l   a0,d0
            beq.s    .no.gi       ; NULL gadget info, call intuition.
            movem.l  a0/a5,-(a7)     ; save some regs.
            move.l   ggi_Window(a0),d0
            beq.s    .no.win
            move.l   d0,a5
            move.l   wd_UserData(a5),a3
.no.win     move.l   _GfxBase(a4),a6
            move.l   ggi_Layer(a0),a5
            move.l   a5,d0
            beq.s    .no.layer      ; no layer, call intuition.
            call     LockLayerRom
            ext.l    d0
            beq.s    .no.lock       ; no lock, return NULL
            call     UnlockLayerRom
.no.layer   movem.l  (a7)+,a0/a5
.no.gi      move.l   _IntuitionBase(a4),a6
            call     ObtainGIRPort
            tst.l    d0
            beq.s    .no.rp
            move.l   d0,a0
            move.l   a3,rp_RP_User(a0)
.no.rp      movem.l  (a7)+,a3/a6
            rts
.no.lock    movem.l  (a7)+,a0/a5
            movem.l  (a7)+,a3/a6
            rts


aguideName  dc.b  'amigaguide.library',0
            ds.w  0

            END

