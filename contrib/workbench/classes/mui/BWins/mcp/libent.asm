
        SECTION BWin,CODE

        NOLIST

        INCLUDE "BWin.mcp_rev.i"
        INCLUDE "exec/libraries.i"
        INCLUDE "exec/resident.i"
        INCLUDE "exec/initializers.i"
        INCLUDE "exec/lists.i"
        INCLUDE "exec/semaphores.i"

        LIST

        XREF    _LinkerDB
        XREF    _LIBNAME
        XREF    ENDCODE

        XREF    _initLib
        XREF    _openLib
        XREF    _expungeLib
        XREF    _closeLib
        XREF    _query

        XDEF    _ID

PRI     EQU 0

start:  moveq   #-1,d0
        rts

romtag:
        dc.w    RTC_MATCHWORD
        dc.l    romtag
        dc.l    ENDCODE
        dc.b    RTF_AUTOINIT
        dc.b    VERSION
        dc.b    NT_LIBRARY
        dc.b    PRI
        dc.l    _LIBNAME
        dc.l    _ID
        dc.l    init

_ID:    VSTRING

        CNOP    0,4

init:   dc.l    LIB_SIZE
        dc.l    funcTable
        dc.l    dataTable
        dc.l    _initLib

V_DEF   MACRO
    dc.w    \1+(*-funcTable)
    ENDM

funcTable:
        DC.W    -1
        V_DEF   _openLib
        V_DEF   _closeLib
        V_DEF   _expungeLib
        V_DEF   nil
        V_DEF   _query
        DC.W    -1

dataTable:
        INITBYTE LN_TYPE,NT_LIBRARY
        INITLONG LN_NAME,_LIBNAME
        INITBYTE LIB_FLAGS,(LIBF_SUMUSED!LIBF_CHANGED)
        INITWORD LIB_VERSION,VERSION
        INITWORD LIB_REVISION,REVISION
        INITLONG LIB_IDSTRING,_ID
        dc.w     0

        CNOP    0,4

nil:    moveq   #0,d0
        rts

        END
