VERSION = 52
REVISION = 2

.macro DATE
.ascii "26.8.2008"
.endm

.macro VERS
.ascii "info.datatype 52.2"
.endm

.macro VSTRING
.ascii "info.datatype 52.2 (26.8.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: info.datatype 52.2 (26.8.2008)"
.byte 0
.endm
