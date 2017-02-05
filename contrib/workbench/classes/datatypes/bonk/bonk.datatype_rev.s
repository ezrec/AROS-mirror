VERSION = 0
REVISION = 2

.macro DATE
.ascii "18.12.2008"
.endm

.macro VERS
.ascii "bonk.datatype 0.2"
.endm

.macro VSTRING
.ascii "bonk.datatype 0.2 (18.12.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: bonk.datatype 0.2 (18.12.2008)"
.byte 0
.endm
