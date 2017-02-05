VERSION = 0
REVISION = 1

.macro DATE
.ascii "5.11.2008"
.endm

.macro VERS
.ascii "amr.datatype 0.1"
.endm

.macro VSTRING
.ascii "amr.datatype 0.1 (5.11.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: amr.datatype 0.1 (5.11.2008)"
.byte 0
.endm
