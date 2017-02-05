VERSION = 0
REVISION = 1

.macro DATE
.ascii "18.12.2008"
.endm

.macro VERS
.ascii "wavpack.datatype 0.1"
.endm

.macro VSTRING
.ascii "wavpack.datatype 0.1 (18.12.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: wavpack.datatype 0.1 (18.12.2008)"
.byte 0
.endm
