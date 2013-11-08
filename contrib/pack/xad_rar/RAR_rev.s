VERSION = 2
REVISION = 6

.macro DATE
.ascii "8.11.2013"
.endm

.macro VERS
.ascii "RAR 2.6"
.endm

.macro VSTRING
.ascii "RAR 2.6 (8.11.2013)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: RAR 2.6 (8.11.2013)"
.byte 0
.endm
