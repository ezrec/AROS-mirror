# $Date: 2012-08-10 13:47:28 +0200 (Fr, 10. Aug 2012) $
# $Revision: 915 $
#############################################################

SHELL	=	/bin/sh
# On OS4 use gmake to build Scalos

##############################################################################

ifndef MACHINE
	MACHINE := $(shell gcc $(CFLAGS) -dumpmachine)
endif

ifeq ($(MAKECMDGOALS),install)
	SVNVERSION := "0000"
endif
ifeq ($(MAKECMDGOALS),clean)
	SVNVERSION := "0000"
endif
ifeq ($(MAKECMDGOALS),nodebug)
	SVNVERSION := "0000"
endif
ifndef SVNVERSION
	SVNVERSION := $(shell svnversion -n)
endif

##############################################################################

ifeq ($(MACHINE), ppc-morphos)

##############################################################################
# MorphOS
##############################################################################

CC 		=	gcc

ifndef GCCVERSION
	GCCVERSION := $(shell $(CC) $(CFLAGS) -dumpversion)
endif

#ifneq ($(GCCVERSION), 4.4.4)
#	 CC 	 =	 gcc4
#	 GCCVERSION := $(shell $(CC) $(CFLAGS) -dumpversion)
#endif

AS 		=	ppc-morphos-as
LD 		=	ppc-morphos-ld
AR		=	ppc-morphos-ar
RANLIB		=	ppc-morphos-ranlib
STRIP		=	ppc-morphos-strip
DUMP		=	ppc-morphos-objdump

CODETYPE	=	PPC

WARNINGS	=	-Wall -Wno-parentheses -Wunused -Wuninitialized -Winline 

ifeq ($(GCCVERSION), 4.4.5)
	WARNINGS	+=	-Wno-pointer-sign
endif
ifeq ($(GCCVERSION), 4.4.4)
	WARNINGS	+=	-Wno-pointer-sign
endif
ifeq ($(GCCVERSION), 4.0.4)
	WARNINGS	+=	-Wno-pointer-sign
endif
CPU		=	-mcpu=604e
#CPU		 +=	 -maltivec

OPTIONS		=	-noixemul -mregnames -mmultiple -mno-prototype -mfused-madd -funsigned-char

OPTIMIZE	=	-O2 -fomit-frame-pointer -fschedule-insns2 -fbranch-count-reg -fno-strict-aliasing

INCLUDES	=	-I$(TOPLEVEL)/ppc-mos-include -I$(TOPLEVEL)/include

DEBUG		=	-g -gstabs

LFLAGS		=       -L $(TOPLEVEL)/ppc-mos-gcc-lib \
			-noixemul -laboxstubs -ldebug

CFLAGS		=	$(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) $(INCLUDES) -I.

SFLAGS		=	--remove-section .comment

DFLAGS		=	--syms --reloc --disassemble-all

OBJDIR		=	.obj_mos
BINDIR		=	.bin_mos

############################################################################

DEFINES		=	-D$(CODETYPE) -D__MORPHOS__ -DAMIGA -DSVN_VERSION="$(SVNVERSION)"

##############################################################################
# End of MorphOS
##############################################################################

else
ifeq ($(MACHINE), ppc-amigaos)

##############################################################################
# AmigaOS4
##############################################################################

CC 		=	ppc-amigaos-gcc
AS 		=	ppc-amigaos-as
LD 		=	ppc-amigaos-ld
AR		=	ppc-amigaos-ar
RANLIB		=	ppc-amigaos-ranlib
STRIP		=	ppc-amigaos-strip
DUMP		=	ppc-amigaos-objdump

CODETYPE	=	PPC

WARNINGS	=	-Wall -Wno-parentheses -Wunused -Wuninitialized

CPU		=

OS4_CRT		=	newlib

OPTIONS		=	-mcrt=$(OS4_CRT)

OPTIMIZE	=	-O2 -fomit-frame-pointer -fschedule-insns2 -fbranch-count-reg -fno-strict-aliasing

INCLUDES	=	-I$(TOPLEVEL)/ppc-aos4-include -I$(TOPLEVEL)/include

DEBUG		=	-g -gstabs

LFLAGS		=       -mcrt=$(OS4_CRT) -L $(TOPLEVEL)/ppc-aos4-gcc-lib \
			-lm -ldebug

CFLAGS		=	$(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) $(INCLUDES) -I.

SFLAGS		=	--remove-section .comment

DFLAGS		=	--syms --reloc --disassemble-all

OBJDIR		=	.obj_os4
BINDIR		=	.bin_os4

############################################################################

OS4LIBBASE	=	-D__NOLIBBASE__
DEFINES		=	-D$(CODETYPE) -D__USE_INLINE__ $(OS4LIBBASE) -Dstccpy=strlcpy \
			-DSVN_VERSION="$(SVNVERSION)"

##############################################################################
# End of AmigaOS4
##############################################################################

else
ifeq ($(MACHINE), i386-aros)

##############################################################################
# i386-AROS
##############################################################################

CC 		=	i386-linux-aros-gcc
AS 		=	i386-linux-aros-as
LD 		=	i386-linux-aros-ld
AR		=	i386-linux-aros-ar
RANLIB		=	i386-linux-aros-ranlib
STRIP		=	i386-linux-aros-strip
DUMP		=	i386-linux-aros-objdump

CODETYPE	=

WARNINGS	=	-Wall -Wno-parentheses -Wunused -Wuninitialized -Wno-pointer-sign

CPU		=

OPTIONS		=	-D__USE_BASETYPE__

OPTIMIZE	=	-O2 -fomit-frame-pointer -fschedule-insns2 -fbranch-count-reg -fno-strict-aliasing

INCLUDES	=	-I$(TOPLEVEL)/include

DEBUG		=	-g -gstabs

LFLAGS		=   -lm

CFLAGS		=	$(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) $(INCLUDES) -I.

SFLAGS		=	--remove-section .comment --strip-unneeded

DFLAGS		=	--syms --reloc --disassemble-all

OBJDIR		=	.obj_i386-aros
BINDIR		=	.bin_i386-aros

############################################################################

DEFINES		=	-DAMIGA -DSVN_VERSION="$(SVNVERSION)"

##############################################################################
# End of i386-aros
##############################################################################

else

##############################################################################
# AmigaOS GCC
##############################################################################

CC 		=	m68k-amigaos-gcc
#AS 		=	m68k-amigaos-as
AS		=	SYS:bin/PhxAss
LD 		=	m68k-amigaos-ld
AR		=	m68k-amigaos-ar
RANLIB		=	m68k-amigaos-ranlib
STRIP		=	m68k-amigaos-strip
DUMP		=	m68k-amigaos-objdump

CODETYPE	=	M68K

WARNINGS	=	-Wall -Wno-parentheses -Wno-unused -Wno-missing-braces \
			-Wno-multichar

CPU		=	-m68020-60

OPTIONS		=	-noixemul -msoft-float -fno-strict-aliasing

OPTIMIZE	=	-O2 -fomit-frame-pointer 

INCLUDES	=	-I$(TOPLEVEL)/include -Igg:include -Iinclude:

DEBUG		=	-g

LFLAGS		=	-noixemul -L $(TOPLEVEL)/68k-gcc-lib \
			-Lgg:lib -Lgg:lib/libnix

CFLAGS		=	$(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) $(INCLUDES) -I.

SFLAGS		=	--strip-all --strip-unneeded --remove-section .comment

DFLAGS	 	=	 --syms --reloc --disassemble-all

ASFLAGS		=	quiet m=68020 linedebug opt=NRQB i=sc:Assembler_Headers/

LIBPATH	 	= 	gg:lib

STARTUP 	= 	$(LIBPATH)/libnix/ncrt0.o 
#STARTUP 	= 	$(LIBPATH)/libnix/detach.o gg:lib/asm_debug.o 

OBJDIR		=	.obj_os3
BINDIR		=	.bin_os3

##############################################################################

DEFINES		=	-D$(CODETYPE) -DSVN_VERSION=$(SVNVERSION)

##############################################################################
# End of AmigaOS GCC
##############################################################################

endif
endif
endif

##############################################################################

#MAKE		 =	 make
CD		=	cd
CP		=	cp
RM		=	rm
ECHO		=	echo
MKDIR		=	mkdir
LN		=	ln
SED		=	sed
CATCOMP		=	CatComp
FLEXCAT		=	flexcat

##############################################################################

TOOLSDIR	=	$(TOPLEVEL)/tools

##############################################################################

FIXDEPS		=	$(TOOLSDIR)/fixdeps

##############################################################################
