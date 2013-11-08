VERSION = 2
REVISION = 5

.macro DATE
.ascii "5.2.2012"
.endm

.macro VERS
.ascii "RAR 2.5"
.endm

.macro VSTRING
.ascii "RAR 2.5 (5.2.2012)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: RAR 2.5 (5.2.2012)"
.byte 0
.endm
