    SECTION "math64_cmp64", code

    XDEF    _sub64

; void __asm sub64 (register __a0 struct EClockVal *dst,
;                   register __a1 struct EClockVal *src);

_sub64:
    addq.l  #4,a1
    addq.l  #4,a0
    move.l  (a1),d0
    sub.l   d0,(a0)         ; dst->lo - src->lo
    subx.l  -(a1),-(a0)     ; dst->hi - src->hi
    rts

    end
