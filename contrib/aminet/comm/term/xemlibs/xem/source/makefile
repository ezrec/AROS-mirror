# Make-Datei

OP			=	obj:
IP			=	dh1:lrc/ownincs/
XP			=	dh1:lrc/code/xem/
AP			=	dh1:lrc/asource/
CP			=	dh1:lrc/csource/

CC       = sc:c/sc
AS       = DH2:DevPac/GenIm2

#CFLAGS	= -ccfiksu -q3e -b1 -r1r -mta -v -Hram:sym/comm.q -Hram:sym/AmigaInclude.sym -j85e86e87e88e89e94e100e -o$(OP)
# Debug=SF
CFLAGS	= LibCode AFP CNest MinC UtilLib StrMer UChar NoErrorRexx MaxErr=3 Data=N Code=N Parm=R CPU=68000 GstImm GST=dh1:gst/AmigaInclude.gst NoStackCheck Err=85,86,87,88,94,100 ObjName=$(OP)
AFLAGS	= -L

VT340OBJ		=	$(OP)VT340_writecon.o\
					$(OP)VT340_funcs.o $(OP)VT340_VT52funcs.o\
					$(OP)VT340_ESCfuncs.o $(OP)VT340_CSIfuncs.o\
					$(OP)VT340_Suppfuncs.o	$(OP)VT340_TEKfuncs.o	$(OP)VT340_transfer.o\
					$(OP)VT340_page.o $(OP)VT340_parseinit.o  $(OP)VT340_hidden.o
#					$(OP)VT340_charloc.o	$(OP)VT340_graphics.o	$(OP)VT340_technical.o	$(OP)VT340_DECsuppl.o	$(OP)VT340_ISOsuppl.o	$(OP)VT340_ASCII.o

BBSOBJ		=	$(OP)BBSfuncs.o $(OP)BBS_ESCfuncs.o\
					$(OP)BBS_CSIfuncs.o $(OP)BBS_Suppfuncs.o $(OP)BBS_hidden.o

CHATTOBJ	=	$(OP)Chattfuncs.o

AMIGAOBJ	=	$(OP)Amigafuncs.o

ASCIIOBJ = $(OP)ASCII_funcs.o $(OP)ASCII_hidden.o

all : $(XP)xemascii.library $(XP)xemvt340.library $(XP)xemchatter.library\
		$(XP)xemamiga.library $(XP)xembbs.library



$(XP)xembbs.library: $(BBSOBJ)
	slink WITH $(CP)BBS/BBSlinkfile
	remlib xembbs.library

$(XP)xemvt340.library: $(VT340OBJ)
	slink WITH $(CP)VT340/VT340_linkfile
	remlib xemvt340.library

$(XP)xemascii.library: $(ASCIIOBJ)
	slink WITH $(CP)ASCII/ASCIIlinkfile
	remlib xemascii.library

$(XP)xemchatter.library: $(CHATTOBJ)
	slink WITH $(CP)Chatter/chattlinkfile
	remlib xemchatter.library

$(XP)xemamiga.library: $(AMIGAOBJ)
	slink WITH $(CP)amiga/amigalinkfile
	remlib xemamiga.library

.c.o:
	$(CC) $(CFLAGS) $*.c
.a.o:
	$(AS) -L -O$@ $*.a



# BBS
# ---
$(OP)BBSfuncs.o		: $(CP)BBS/BBSfuncs.c		$(IP)BBSConsole.h	$(IP)xem.h
$(OP)BBS_ESCfuncs.o	: $(CP)BBS/BBS_ESCfuncs.c	$(IP)BBSConsole.h	$(IP)xem.h
$(OP)BBS_CSIfuncs.o	: $(CP)BBS/BBS_CSIfuncs.c	$(IP)BBSConsole.h	$(IP)xem.h
$(OP)BBS_Suppfuncs.o	: $(CP)BBS/BBS_Suppfuncs.c	$(IP)BBSConsole.h	$(IP)xem.h
$(OP)BBS_hidden.o		: $(CP)BBS/BBS_hidden.c		$(IP)BBSConsole.h	$(IP)xem.h


# VT340
# -----
$(OP)VT340_funcs.o		: $(CP)VT340/VT340_funcs.c			$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_writecon.o	: $(CP)VT340/VT340_writecon.c		$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_transfer.o	: $(CP)VT340/VT340_transfer.c		$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_VT52funcs.o	: $(CP)VT340/VT340_VT52funcs.c	$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_ESCfuncs.o	: $(CP)VT340/VT340_ESCfuncs.c		$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_CSIfuncs.o	: $(CP)VT340/VT340_CSIfuncs.c		$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_TEKfuncs.o	: $(CP)VT340/VT340_TEKfuncs.c		$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_Suppfuncs.o	: $(CP)VT340/VT340_Suppfuncs.c	$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_page.o			: $(CP)VT340/VT340_page.c			$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_parseinit.o	: $(CP)VT340/VT340_parseinit.c	$(IP)VT340Console.h	$(IP)xem.h
$(OP)VT340_hidden.o		: $(CP)VT340/VT340_hidden.c		$(IP)VT340Console.h	$(IP)xem.h
#$(OP)VT340_graphics.o	: $(AP)VT340_graphics.a
#$(OP)VT340_technical.o	: $(AP)VT340_technical.a
#$(OP)VT340_DECsuppl.o	: $(AP)VT340_DECsuppl.a
#$(OP)VT340_ISOsuppl.o	: $(AP)VT340_ISOsuppl.a
#$(OP)VT340_ASCII.o		: $(AP)VT340_ASCII.a
#$(OP)VT340_charloc.o	: $(AP)VT340_charloc.a
#$(OP)VT220_stretch.o	: $(AP)VT220_stretch.a
#	asm -o$(OP) -iINCLUDE: $(AP)VT220_stretch.a



# ASCII
# -----
$(OP)ASCII_funcs.o : $(CP)ASCII/ASCII_funcs.c	$(IP)ASCIIConsole.h	$(IP)xem.h
	$(CC) $(CFLAGS) $(CP)ASCII/ASCII_funcs.c
$(OP)ASCII_hidden.o : $(CP)ASCII/ASCII_hidden.c $(IP)ASCIIConsole.h	$(IP)xem.h


# AmigaANSI
# ---------
$(OP)Amigafuncs.o : $(CP)Amiga/Amigafuncs.c	$(IP)AmigaConsole.h	$(IP)xem.h
	$(CC) $(CFLAGS) $(CP)Amiga/Amigafuncs.c
$(OP)Amiga_lib.o	: $(CP)Amiga/Amiga_lib.a


# Chatter
# -------
$(OP)Chattfuncs.o : $(CP)Chatter/Chattfuncs.c	$(IP)ChattConsole.h	$(IP)xem.h
	$(CC) $(CFLAGS) $(CP)Chatter/Chattfuncs.c
$(OP)Chatter_lib.o	: $(CP)Chatter/Chatter_lib.a


$(OP)FastText.o :  $(AP)FastText.a
