    SECTION "i2m_long", code

    XDEF    _i2m_long
    XDEF    @i2m_long

    ; d0    long
_i2m_long:
@i2m_long:
    rol.w   #8,d0
    swap    d0
    rol.w   #8,d0
    rts

    end

