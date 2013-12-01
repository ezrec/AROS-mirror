# $Date: 2011-07-31 23:57:02 +0200 (So, 31. Jul 2011) $
# $Revision: 814 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

COMMON_DIR	= $(TOPLEVEL)/common/Plugin

INCLUDES	+= -I$(COMMON_DIR)

vpath 	%.c	$(COMMON_DIR)

##############################################################################

include $(COMMON_DIR)/config.mk

##############################################################################

# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=	#

LFLAGS	:=	-nostartfiles \
		-lpng \
		-lz \
		-lmempools \
		$(LFLAGS) \
#		-Wl,-Map,pngpicture.map \
#		--verbose


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS  +=      -nostartfiles \
		-lpng \
		-lz \
		-lm
#


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS  +=      -nostartfiles \
		-lpng \
		-lz \
		-lm
#


else

###############################################################################
# AmigaOS

INCLUDES	+=	#

LFLAGS	+=	-lmempools \
		-lpng \
		-lz \
		-ldebug \
		-lamiga21 \
		-lamiga \
		-lstubs

endif
endif
endif
