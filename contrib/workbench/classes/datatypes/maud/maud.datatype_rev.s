VERSION = 52
REVISION = 1

.macro DATE
.ascii "2.5.2009"
.endm

.macro VERS
.ascii "maud.datatype 52.1"
.endm

.macro VSTRING
.ascii "maud.datatype 52.1 (2.5.2009)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: maud.datatype 52.1 (2.5.2009)"
.byte 0
.endm
