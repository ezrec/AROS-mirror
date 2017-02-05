VERSION = 52
REVISION = 3

.macro DATE
.ascii "26.7.2008"
.endm

.macro VERS
.ascii "icns.datatype 52.3"
.endm

.macro VSTRING
.ascii "icns.datatype 52.3 (26.7.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: icns.datatype 52.3 (26.7.2008)"
.byte 0
.endm
