# TheBar MCP makefile

# host
ifndef (HOST)
  HOST = $(shell uname)
  ifeq ($(HOST),AmigaOS)
    ifeq ($(shell uname -m),powerpc)
      HOST = AmigaOS4
    endif
    ifeq ($(shell uname -m),ppc)
      HOST = AmigaOS4
    endif
  endif
endif

# os
ifndef (OS)
  ifeq ($(HOST),AmigaOS4)
    OS = os4
  else
  ifeq ($(HOST),AmigaOS)
    OS = os3
  else
  ifeq ($(HOST),MorphOS)
    OS = mos
  else
  ifeq ($(HOST),Aros)
    OS = aros
  else
    OS = os4
  endif
  endif
  endif
  endif
endif

# debug
ifeq ($(DEBUG),1)
 override DEBUG = -DDEBUG -O0
else
ifeq ($(DEBUG),yes)
 override DEBUG = -DDEBUG -O0
else
 override DEBUG =
endif
endif

# commands
MKDIR  = mkdir
CP     = cp
RM     = rm -f
FLUSH  = avail flush
DUMP   = objdump
CHMOD  = chmod u+x
FC     = flexcat

# override commands by os
ifeq ($(HOST),AmigaOS4)
  # AmigaOS4
  RM    = delete force
  MKDIR = makedir
else
ifeq ($(HOST),AmigaOS)
  # AmigaOS3
  RM    = delete force
  MKDIR = makedir
else
ifeq ($(HOST),MorphOS)
  # MorphOS
endif
endif
endif

# common definitions
TNAME   = TheBar.mcp
ODIR    = .obj_$(OS)
BINDIR  = bin_$(OS)
VPATH   = $(ODIR)
TARGET  = $(BINDIR)/$(TNAME)
DEPFILE = makefile.dep


# common compiler/linker flags
WARN     = -W -Wall
OPTFLAGS = -O3 -funroll-loops
DEBUGSYM = -gstabs
CFLAGS   = -c -I../include $(CPU) $(WARN) $(OPTFLAGS) -fomit-frame-pointer $(DEBUGSYM) $(DEBUG)
LDFLAGS  = $(CPU) $(DEBUGSYM) -nostartfiles
LDLIBS   =

# objects
OBJS      = library.o utils.o loc.o class.o debug.o
EXTRAOBJS = coloradjust.o backgroundadjust.o penadjust.o popbackground.o poppen.o
# these are for dependencies generation
ALLOBJS   = $(OBJS)
ALLOBJS  += $(EXTRAOBJS)

# options by os
ifeq ($(OS),os4)
  # AmigaOS4
  CC         = ppc-amigaos-gcc
  STRIP      = ppc-amigaos-strip
  OBJDUMP    = ppc-amigaos-objdump
  CRT        = newlib
  CPU        = -mcpu=powerpc
  CFLAGS    += -mcrt=$(CRT) -D__USE_INLINE__ -D__NEW_TIMEVAL_DEFINITION_USED__ -DNO_PPCINLINE_STDARG -Wa,-mregnames
  LDFLAGS   += -mcrt=$(CRT)
  M68KSTUBS  = $(ODIR)/mccclass_68k.o
  DESTDIR    = MUI:Libs/MUI
else
ifeq ($(OS),os3)
  # AmigaOS3
  CC       = m68k-amigaos-gcc
  STRIP    = m68k-amigaos-strip
  OBJDUMP  = m68k-amigaos-objdump
  CPU	   = -m68020-60 -msoft-float
  CFLAGS  += -noixemul -DNO_INLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug -lmui
  DESTDIR  = MUI:Libs/MUI
  OBJS    += $(EXTRAOBJS)
else
ifeq ($(OS),mos)
  # MorphOS
  CC       = ppc-morphos-gcc
  STRIP    = ppc-morphos-strip
  OBJDUMP  = ppc-morphos-objdump
  CPU      = -mcpu=powerpc
  CFLAGS  += -noixemul -nostartfiles -DUSE_INLINE_STDARG
  LDFLAGS += -noixemul
  LDLIBS  += -ldebug
  DESTDIR  = Sys:Classes/MUI
else
ifeq ($(OS),aros)
  # AROS
  CC      = i686-aros-gcc
  STRIP   = i686-aros-strip
  OBJDUMP = i686-aros-objdump
  CPU     =
  DESTDIR = Sys:Classes/Zune
endif
endif
endif
endif

# objs
FOBJS = $(addprefix $(ODIR)/,$(OBJS))

# main target
.PHONY: all
ifeq ($(wildcard $(DEPFILE)),$(DEPFILE))
all: $(BINDIR) $(ODIR) $(M68KSTUBS) $(TARGET)
    # great, we have a dependecies file, let's make our target
else
all: depend
    # no dependecies, create it and then call make again
	@make --no-print-directory all
endif

# create binaries directory
$(BINDIR):
	@echo  "  Creating $(OS) binaries directory"
	@$(MKDIR) $(BINDIR)

# create objects directory
$(ODIR):
	@echo  "  Creating $(OS) objects directory"
	@$(MKDIR) $(ODIR)

$(M68KSTUBS): ../include/mccclass_68k.c
	@echo "  CC $(notdir $<)"
	@$(CC) $(CFLAGS) $< -o $@

# build
$(TARGET): $(OBJS)
	@echo "  LD $(notdir $@)"
	@$(CC) $(LDFLAGS) -o $@.db $(FOBJS) $(M68KSTUBS) $(LDLIBS) -Wl,-Map,$@.map
	@$(STRIP) --remove-section=.comment -o $@ $@.db

# compile
$(OBJS):
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $< -o $(ODIR)/$@

# locale
loc.h: TheBar.cd loc_h.sd
	@echo "  Generating locale..."
	@$(FC) TheBar.cd loc.h=loc_h.sd

# clean
.PHONY: clean
clean:
	@echo "  => Cleaning..."
	@$(RM) $(FOBJS) $(M68KSTUBS)
	@$(RM) $(BINDIR)/$(TNAME) $(BINDIR)/$(TNAME).db $(BINDIR)/$(TNAME).map 
	@echo "  <= Done."

# cleanall
.PHONY: cleanall
cleanall:
	@echo "  => Cleaning all..."
	@for i in aros os3 os4 mos; do\
		$(RM) $(addprefix .obj_$$i/,$(OBJS)) $(M68KSTUBS);\
		$(RM) bin_$$i/$(TNAME) bin_$$i/$(TNAME).db bin_$$i/$(TNAME).map;\
	done
	@echo "  <= Done."

# install
.PHONY: install
install: all
	@echo "  => Installing..."
	@$(CP) $(BINDIR)/$(TNAME) $(DESTDIR)
	@echo "  <= Done."

# catalogs
.PHONY: catalogs
catalogs:
	@echo "  => Generating catalogs..."
	@for i in $(patsubst %.ct,%,$(subst locale/,,$(wildcard locale/*.ct))); do\
		$(FC) TheBar.cd locale/$$i.ct CATALOG "locale/TheBar_"$$i"_catalog";\
	done
	@echo "  <= Done."

.PHONY: help
help:
	@echo "Cleaning targets:"
	@echo "  clean		- Remove OS generated files"
	@echo "  cleanall	- Remove all generated files"
	@echo ""
	@echo "Other generic targets:"
	@echo "  all		- Build $(TNAME)"
	@echo "  catalogs	- Build all available catalogs"
	@echo ""
	@echo "Install targets:"
	@echo "  install	- Install $(TNAME)"
	@echo ""
	@echo "Parameters:"
	@echo "  make OS=aros|os3|os4|morphos => build $(TNAME) in OS format"
	@echo "  make DEBUG=1|yes => build $(TNAME) with debugging informations"
	@echo ""


# create the dependencies file
.PHONY: depend
depend: loc.h
	@echo "  => Generating dependencies, takes about 8 seconds on *Amiga*, 2 seconds on gentoo x86..."
	@$(CC) -MM $(CFLAGS) $(patsubst %.o,%.c, $(ALLOBJS)) >$(DEPFILE)
	@echo "  <= Done."

# include dependencies file
-include $(DEPFILE)

