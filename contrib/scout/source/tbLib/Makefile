##
##

# Evaluate explicit target specification
ifneq ($(OS),)
    PREFIX   := $(OS)-
    HOST     := $(subst -, ,$(OS))
    HOST_CPU := $(word 1,$(HOST))
    HOST_OS  := $(word 2,$(HOST))
else
    PREFIX  :=
    HOST_OS := $(shell uname)
endif

# Translate canonical OS name to lowercase
ifeq ($(HOST_OS),AROS)
    HOST_OS := aros
    # TODO: detect CPU here for self-hosted build under AROS
endif
ifeq ($(HOST_OS),MorphOS)
    HOST_OS  := morphos
    HOST_CPU := ppc
endif

# target-dependent options
ifeq ($(HOST_OS),aros)
    NOIXEMUL :=
else
    NOIXEMUL := -noixemul
endif
ifeq ($(HOST_CPU),ppc)
    CPU := -mcpu=750
else
    CPU :=
endif

TARGET = libtblib.a

CC = $(PREFIX)gcc
AR = $(PREFIX)ar

OBJDIR=             .obj

REDEFINE=
OPTIMIZE=           -O3
DEBUG   =
CFLAGS  =           $(USER_CFLAGS) -Wall $(NOIXEMUL) $(CPU) -I../include $(REDEFINE) $(DEBUG) -c


OBJS = $(OBJDIR)/ports_AllocPort.o \
	$(OBJDIR)/ports_FreePort.o \
	$(OBJDIR)/ports_AllocSig.o \
	$(OBJDIR)/ports_FreeSig.o \
	$(OBJDIR)/ports_SafePutMsg.o \
	$(OBJDIR)/timer_TimerDelay.o \
	$(OBJDIR)/timer_StartTimer.o \
	$(OBJDIR)/timer_AbortTimer.o \
	$(OBJDIR)/string_b2cstr.o \
	$(OBJDIR)/string_b2cstrn.o \
	$(OBJDIR)/string_c2bstr.o \
	$(OBJDIR)/string_snprintf.o \
	$(OBJDIR)/string_sprintf.o \
	$(OBJDIR)/string_strcat.o \
	$(OBJDIR)/string_strcatn.o \
	$(OBJDIR)/string_strdup.o \
	$(OBJDIR)/string_strdup_pool.o \
	$(OBJDIR)/string_strsep.o \
	$(OBJDIR)/string_parse_string.o \
	$(OBJDIR)/i2m_word_c.o \
	$(OBJDIR)/i2m_long_c.o \
	$(OBJDIR)/math64_add64_c.o \
	$(OBJDIR)/math64_sub64_c.o \
	$(OBJDIR)/math64_cmp64_c.o \
	$(OBJDIR)/envvar_GetEnvVar.o \
	$(OBJDIR)/envvar_SetEnvVar.o \
	$(OBJDIR)/mui_MakeButton.o \
	$(OBJDIR)/mui_MakeCheckmark.o \
	$(OBJDIR)/errorreq_ErrorMsg.o \
	$(OBJDIR)/library_CheckLibVersion.o \
	$(OBJDIR)/library_CheckLibVersionName.o

INCLUDES= tblib.h \
	SDI_compiler.h

all: $(OBJDIR) $(TARGET)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINE) $< -o $@

$(OBJDIR):
	@mkdir $(OBJDIR)

$(TARGET): $(OBJS)
	$(AR) -r libtblib.a $(OBJS)

clean:
	-rm -f $(OBJS) $(TARGET) quiet

realclean:
	@make clean

date:
	@;

