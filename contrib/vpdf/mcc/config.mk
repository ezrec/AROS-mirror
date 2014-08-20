
ifeq "$(shell test -f /gg/bin/ppc-morphos-gcc-3.4.6 && echo YES)" "YES"
	CC3 = ppc-morphos-gcc-3.4.6 -mabi=altivec -maltivec
else
	CC3=ppc-morphos-gcc
endif

CC= ppc-morphos-gcc
AS= ppc-morphos-as
AR= ppc-morphos-ar rcs
LD= ppc-morphos-ld

CFLAGS = -g -O3 -Wall -fstrength-reduce -mfused-madd -mmultiple -frerun-loop-opt -fexpensive-optimizations -fstrict-aliasing -fschedule-insns2 -noixemul -ffast-math -mfused-madd -mmultiple -fomit-frame-pointer -mcpu=750
CFLAGS_LESS = -g -O2 -Wall -fstrength-reduce -mfused-madd -mmultiple -frerun-loop-opt -fexpensive-optimizations -fschedule-insns2 -noixemul -ffast-math -mfused-madd -mmultiple -fomit-frame-pointer -mcpu=750
INCLUDES = -I../showgirls/system -I.. -I../exif -I../../showgirls -I../../lairecvs/morphos/morphoswb/apps/showgirls/system/ -I../../lairecvs/morphos/morphoswb/apps/showgirls/
LFLAGS=

# For building with the public SDK
INCLUDES += -I../include

