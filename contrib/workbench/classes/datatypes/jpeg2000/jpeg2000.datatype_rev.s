VERSION = 0
REVISION = 2

.macro DATE
.ascii "29.7.2008"
.endm

.macro VERS
.ascii "jpeg2000.datatype 0.2"
.endm

.macro VSTRING
.ascii "jpeg2000.datatype 0.2 (29.7.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: jpeg2000.datatype 0.2 (29.7.2008)"
.byte 0
.endm
