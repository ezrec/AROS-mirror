OPT MODULE
OPT NOWARN

EXPORT PROC sys_SGlob()
   LEA      __Global(PC),A0
   MOVE.L   A4,(A0)
ENDPROC
EXPORT PROC sys_RGlob()
   LEA      __Global(PC),A0
   MOVEA.L  (A0),A4
ENDPROC
EXPORT PROC sys_global()
   LEA      __Global(PC),A0
   MOVE.L   (A0),D0
ENDPROC D0
/*
PROC sys_GetGlob()
   LEA      __Glob(PC),A0
   MOVE.L   (A0),D0
ENDPROC D0
PROC sys_SetGlob(x)
   MOVE.L   x,D0
   LEA      __Glob(PC),A0
   MOVE.L   D0,(A0)
ENDPROC
PROC sys_CreateGlobal()
   MOVE.L   #512,D0
   MOVE.L   #$10000,D1
   MOVEA.L  4,A6              -> execbase

   JSR      -$2AC(A6)         -> AllocVec(size,attr)(d0,d1)
   TST.L    D0
   BEQ      __Failed
   MOVE.L   D0,-(A7)          -> <S> new_glob start

   MOVEA.L  A4,A0
   SUBA.L   #512,A0         -> A0 := old_glob start
   MOVEA.L  D0,A1             -> A1 := new_glob start
   MOVE.L   #512,D0

   JSR      -$270(A6)         -> CopyMem(source,dest,size)(a0,a1,d0)

   LEA      __OldGlob(PC),A0
   MOVE.L   A4,(A0)
   MOVE.L   (A7)+,A4          -> <R> new_glob start
   ADDA.L   #512,A4         -> A4 := new_glob end
   MOVE.L   A4,D0
   LEA      __Glob(PC),A0
   MOVE.L   D0,(A0)           ; __Failed:
ENDPROC D0
PROC sys_RestoreGlobal()
   LEA      __OldGlob(PC),A0
   MOVEA.L  (A0),A1           -> A1 := old_glob
   MOVEA.L  A4,A0             -> A0 := new_glob
   MOVE.L   #512,D0           -> D0 := glob size
   SUBA.L   D0,A0             -> A0 := new_glob start
   MOVE.L   A1,-(A7)          -> <S> old_glob end
   MOVE.L   A0,-(A7)          -> <S> new_glob start
   SUBA.L   D0,A1             -> A1 := old_glob start

   MOVEA.L  4,A6              -> exec.library

   JSR      -$270(A6)         -> CopyMem(source,dest,size)(a0,a1,d0)
   MOVE.L   (A7)+,A1          -> <R> new_glob start
   JSR      -$2B2(A6)         -> FreeVec(memblock)(a1)

   MOVE.L   (A7)+,A4          -> <R> old_glob end
ENDPROC
__OldGlob: LONG  NIL
*/
__Global:   LONG  NIL
