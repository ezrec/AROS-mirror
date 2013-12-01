# $Date: 2011-08-10 15:09:09 +0200 (Mi, 10. Aug 2011) $
# $Revision: 833 $
##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

ICONOBJMCC_DIR	= $(TOPLEVEL)/common/IconobjectMCC
AMISSL_INC_DIR	= /gg/amissl-v3-sdk/include/include_h

SCALOS_LOCALE	= $(OBJDIR)/Updater_Locale.h

CFLAGS += -D SCALOSLOCALE=$(SCALOS_LOCALE)

##############################################################################
# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \
			-I$(AMISSL_INC_DIR)\

LFLAGS	+=	\
		-lcurl \
		-lz \


else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \

LFLAGS	+=	-lauto \
		-lcurl \
		-lamisslauto \
		-lz \

else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

INCLUDES	+=      -I$(ICONOBJMCC_DIR) \

DEFINES	+= -DSTATIC_SSL -D__BSD_VISIBLE -DMUI_OBSOLETE

LFLAGS	+=	 \
		-lcurl \
		-lssl \
		-lcrypto \
		-lz \

else

###############################################################################
# AmigaOS

INCLUDES	+=      -I$(AMISSL_INC_DIR)\

LFLAGS	+=	-nodefaultlibs \
		-lcurl \
		-lamissl \
		-lz \
		-lm2 \
		-lgcc \
		-lc \
		-ldebug \
		-lamiga \
		-lstubs \

CFLAGS		+=	-Dfd_set=APTR


endif
endif
endif
