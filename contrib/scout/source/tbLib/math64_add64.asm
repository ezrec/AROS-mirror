    SECTION "math64_add64", code

    XDEF    _add64

; void __asm add64 (register __a0 struct EClockVal *dst,
;                   register __a1 struct EClockVal *src);

_add64:
    addq.l  #4,a1
    addq.l  #4,a0
    move.l  (a1),d0
    add.l   d0,(a0)
    addx.l  -(a1),-(a0)
    rts

    end
