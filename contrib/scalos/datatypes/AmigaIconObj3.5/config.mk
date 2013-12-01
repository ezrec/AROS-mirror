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

LFLAGS	+=	-nostartfiles \
		-lmempools \
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS	+=	-nostartfiles \

else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS	+=	-nostartfiles -larossupport \

else

###############################################################################
# AmigaOS

LFLAGS	+=	-lamiga21 -ldebug -lmcpgfx -lnix -lamiga -lstubs

endif
endif
endif
