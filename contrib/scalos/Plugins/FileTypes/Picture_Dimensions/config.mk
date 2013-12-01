# $Date: 2011-07-31 22:05:01 +0200 (So, 31. Jul 2011) $
# $Revision: 813 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

COMMON_DIR	= $(TOPLEVEL)/common/Plugin

INCLUDES	+= -I$(COMMON_DIR)

SCALOS_LOCALE	= $(OBJDIR)/PictureDimensions_locale.h

CFLAGS 		+= -D SCALOSLOCALE=$(SCALOS_LOCALE)

vpath 	%.c	$(COMMON_DIR)

##############################################################################

include $(COMMON_DIR)/config.mk

##############################################################################

# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=	#

LFLAGS	+=	-nostartfiles \
		-lmempools
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS  +=      -nostartfiles \
#


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS  +=      -nostartfiles \
#


else

###############################################################################
# AmigaOS

INCLUDES	+=	#

LFLAGS	+=	-lscalos \
		-lpreferences \
		-liconobject \
		-lmempools \
		-ldebug \
		-lmui \
		-lstack \
		-lnix \
		-lnixmain \
		-lamiga21 \
		-lamiga \
		-lstubs \

endif
endif
endif
