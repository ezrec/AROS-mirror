VERSION = 50
REVISION = 1

.macro DATE
.ascii "27.2.2006"
.endm

.macro VERS
.ascii "au.datatype 50.1"
.endm

.macro VSTRING
.ascii "au.datatype 50.1 (27.2.2006)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: au.datatype 50.1 (27.2.2006)"
.byte 0
.endm
