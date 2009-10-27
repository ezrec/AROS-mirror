VERSION = 37
REVISION = 296

.macro DATE
.ascii "17.12.2006"
.endm

.macro VERS
.ascii "Scout 37.296"
.endm

.macro VSTRING
.ascii "Scout 37.296 (17.12.2006)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: Scout 37.296 (17.12.2006)"
.byte 0
.endm
