    SECTION "i2m_word", code

    XDEF    _i2m_word
    XDEF    @i2m_word

    ; d0    word
_i2m_word:
@i2m_word:
    rol.w   #8,d0
    rts

    end

