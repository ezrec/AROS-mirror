
#CC= ppc-morphos-gcc
CC= ppc-morphos-gcc-4
#CXX= ppc-morphos-g++-2.95.3
CXX= ppc-morphos-g++-4
AS= ppc-morphos-as
AR= ppc-morphos-ar rcs
LD= ppc-morphos-ld

# do NOT include optimization level here
CFLAGS = -DHAVE_CONFIG_H -D_NO_PPCINLINE -D__MORPHOS_SHAREDLIBS -DHAVE_CONFIG_H -Wno-write-strings -Wall -fstrength-reduce -mfused-madd -mmultiple -frerun-loop-opt -fstrict-aliasing -noixemul -ffast-math -mfused-madd -mmultiple -fomit-frame-pointer -mcpu=750
CPPFLAGS = -DHAVE_CONFIG_H -D_NO_PPCINLINE -D__MORPHOS_SHAREDLIBS -DHAVE_CONFIG_H -Wno-write-strings -Wall -fstrength-reduce -mfused-madd -mmultiple -frerun-loop-opt -fstrict-aliasing -noixemul -ffast-math -mfused-madd -mmultiple -fomit-frame-pointer -mcpu=750
CFLAGS_LESS =  -Wall -fstrength-reduce -mfused-madd -mmultiple -frerun-loop-opt -fexpensive-optimizations -fschedule-insns2 -noixemul -ffast-math -mfused-madd -mmultiple -fomit-frame-pointer -mcpu=750
GLOBAL_INCLUDES = -I.. -I. -Iincludes/ -I../../ -I../../../../libs/cairo/cairo-src/MorphOS/include/cairo -I../../../../libs/cairo/cairo-src/MorphOS/include \
 -I../../../../../include  -I../../../../libs/freetype/library/include -I../../../../libs/fontconfig/MorphOS/include -I../../../../libs/freetype/include
LFLAGS=





