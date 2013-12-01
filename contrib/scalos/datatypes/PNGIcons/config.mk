# $Date: 2012-08-10 13:47:28 +0200 (Fr, 10. Aug 2012) $
# $Revision: 915 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

MKDIR   	= mkdir -p #makedir force
DT_DIR		= scalos:IconDatatypes/datatypes

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

LFLAGS	:=      -nostartfiles \
		-lpng \
		-lz \
		-lmempools \
		$(LFLAGS) \
#		-Wl,-Map,png.map \
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS	+=      -nostartfiles \
		-lpng \
		-lz \
		-lm

else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS	+=      -nostartfiles \
		-larossupport \
		-lpng \
		-lz \
		-lm

else

###############################################################################
# AmigaOS

LFLAGS	+=	-ldebug -liconobject -lcybergraphics \
		-lmcpgfx -lz \
		-lnix -lamiga21 -lamiga -lstubs

endif
endif
endif
