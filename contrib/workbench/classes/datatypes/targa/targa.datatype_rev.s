VERSION = 0
REVISION = 2

.macro DATE
.ascii "28.11.2006"
.endm

.macro VERS
.ascii "targa.datatype 0.2"
.endm

.macro VSTRING
.ascii "targa.datatype 0.2 (28.11.2006)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: targa.datatype 0.2 (28.11.2006)"
.byte 0
.endm
