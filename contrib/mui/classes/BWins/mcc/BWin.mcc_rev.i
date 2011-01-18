VERSION EQU 20
REVISION EQU 5
DATE MACRO
    dc.b '7.5.2003'
    ENDM
VERS MACRO
    dc.b 'BWin.mcc 20.5'
    ENDM
VSTRING MACRO
    dc.b '$VER: BWin.mcc 20.5 (7.5.2003) 2002 Alfonso Ranieri <alforan@tin.it>',13,10,0
    ENDM
VERSTAG MACRO
    dc.b 0,'$VER: BWin.mcc 20.5 (7.5.2003)',0
    ENDM
