
OBJS = \
		LibWrapper.o					\
		Main.o 							\
		Various.o               	\
		Drive.o    						\
		Commands.o              	\
		Pages.o							\
		Disc_CD_ROM.o 					\
		Disc_CD_R.o						\
		Disc_CD_RW.o					\
   	Disc_DVD_ROM.o					\
   	Disc_DVD_PlusR.o				\
   	Disc_DVD_PlusRW.o				\
   	Disc_DVD_MinusR.o				\
   	Disc_DVD_MinusRW.o			\
   	Disc_DVD_RAM.o          	\
   	Disc_Generic.o          	\
   	TOC_Elements.o          	\
   	Humming.o						\
		Config.o							\
		CfgVendors.o					\
		CfgIOCodes.o					\
		CfgDrivers.o					\
		CfgHardware.o					\
		CfgCDInfo.o						\
		CfgIOMeas.o						\
		OptItem.o						\
		OptSession.o					\
		OptDisc.o						\
		OptTrack.o						\
		OptIndex.o						\
		CDText.o

   	
Program			= Optical

TARGETS			= aros m68k morphos os4
OBJDIRS			= 

INCOPTS			= -I..
LIBOPTS			= -lGeneric -nostdlib -lC -lLibrary

OWNSTARTUP		= 1
AROS_LIBOPTS	= -L../Generic/$(AROSBIN)
M68K_LIBOPTS	= -L../Generic/$(M68KBIN) 
MOS_LIBOPTS		= -L../Generic/$(MORPHOSBIN)
OS4_LIBOPTS		= -L../Generic/$(OS4BIN) 

BUILD_LIBRARY	= 0

	include ../@Own/Makefile.mk
