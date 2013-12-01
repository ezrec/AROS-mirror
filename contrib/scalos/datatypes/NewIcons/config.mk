# $Date: 2010-04-14 18:00:07 +0200 (Mi, 14. Apr 2010) $
# $Revision: 563 $
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

###############################################################################
# AmigaOS

LFLAGS	+=	-ldebug -lamiga -lpreferences -lcybergraphics -lnix 

endif
