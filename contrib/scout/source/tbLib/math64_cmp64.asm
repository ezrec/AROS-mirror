    SECTION "math64_cmp64", code

    XDEF    _cmp64

; int __asm cmp64 (register __a0 struct EClockVal *dst,
;                  register __a1 struct EClockVal *src);

_cmp64:
    cmpm.l  (a1)+,(a0)+     ; (a0)-(a1), dst->hi - src->hi
    bne.b   1$
    cmpm.l  (a1)+,(a0)+     ; dst->lo - src->lo
    bne.b   1$
    moveq   #0,d0           ; dst=src
    rts
1$:
    bpl.b   2$
    moveq   #1,d0           ; dst<src
    rts
2$:
    moveq   #-1,d0          ; dst>src
    rts

    end
