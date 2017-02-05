VERSION = 0
REVISION = 3

.macro DATE
.ascii "28.11.2006"
.endm

.macro VERS
.ascii "pcx.datatype 0.3"
.endm

.macro VSTRING
.ascii "pcx.datatype 0.3 (28.11.2006)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: pcx.datatype 0.3 (28.11.2006)"
.byte 0
.endm
