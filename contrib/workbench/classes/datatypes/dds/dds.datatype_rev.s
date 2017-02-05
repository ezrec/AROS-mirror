VERSION = 52
REVISION = 2

.macro DATE
.ascii "19.6.2008"
.endm

.macro VERS
.ascii "dds.datatype 52.2"
.endm

.macro VSTRING
.ascii "dds.datatype 52.2 (19.6.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: dds.datatype 52.2 (19.6.2008)"
.byte 0
.endm
