VERSION = 52
REVISION = 1

.macro DATE
.ascii "29.7.2008"
.endm

.macro VERS
.ascii "ico.datatype 52.1"
.endm

.macro VSTRING
.ascii "ico.datatype 52.1 (29.7.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: ico.datatype 52.1 (29.7.2008)"
.byte 0
.endm
