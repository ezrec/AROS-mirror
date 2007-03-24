VERSION EQU 15
REVISION EQU 4
DATE MACRO
    dc.b '25.1.2002'
    ENDM
VERS MACRO
    dc.b 'Urltext.mcp 15.4'
    ENDM
VSTRING MACRO
    dc.b '$VER: Urltext.mcp 15.4 (25.1.2002) 2000-2002 Alfonso Ranieri <alforan@tin.it>',13,10,0
    ENDM
VERSTAG MACRO
    dc.b 0,'$VER: Urltext.mcp 15.4 (25.1.2002)',0
    ENDM
