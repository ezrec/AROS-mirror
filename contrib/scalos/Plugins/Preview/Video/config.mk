##############################################################################

include $(TOPLEVEL)/config.mk
include $(TOPLEVEL)/rules.mk

##############################################################################

COMMON_DIR	= $(TOPLEVEL)/common/Plugin
FFMPEG_DIR	= $(TOPLEVEL)/include/ffmpeg

INCLUDES	+= -I$(COMMON_DIR) -I$(FFMPEG_DIR)

vpath 	%.c	$(COMMON_DIR)

##############################################################################

LFLAGS	:=	\
		-lavcodec \
		-lavformat \
		-lavcodec \
		-lavutil \
		-lz \
		$(LFLAGS) \

##############################################################################

include $(COMMON_DIR)/config.mk

##############################################################################

# Check gcc

ifeq ($(MACHINE), ppc-morphos)

###############################################################################
# MorphOS

INCLUDES	+=	#

LFLAGS	:=	-nostartfiles \
		$(LFLAGS) \
		-lmempools \
#		 -Wl,-Map,video.map \
#		--verbose \
#		-nostdlib


COMPAT_OBJS =	$(OBJDIR)/compat_mos.o

else
ifeq ($(MACHINE), ppc-amigaos)

###############################################################################
# AmigaOS4

LFLAGS  +=      -nostartfiles \
		-lm
#

COMPAT_OBJS =	$(OBJDIR)/compat_os4.o


else
ifeq ($(MACHINE), i386-aros)

###############################################################################
# i386-aros

LFLAGS  +=      -nostartfiles \
		-lm
#

COMPAT_OBJS =	$(OBJDIR)/compat_mos.o


else

###############################################################################
# AmigaOS

INCLUDES	+=	#

LFLAGS	+=	-specs=gg:etc/specs -nostdlib\
		-lm2 \
		-lgcc \
		-lmempools \
		-lnix\
		-lc \
		-ldebug \
		-lamiga21 \
		-lamiga \
		-lstubs \
		-Wl,-Map,video.map \
#		--verbose \


COMPAT_OBJS =	$(OBJDIR)/compat_68k.o

endif
endif
endif
