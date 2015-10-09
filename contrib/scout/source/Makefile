# You may call this makefile with:
# make OS=<target>
#  where target is your crosscompiler prefix (like ppc-morphos,
#
# Note that you may omit OS specification, in this case your host OS will be detected

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
    SYSLIB   := -larossupport -larosm -lmui -lrom
else
    NOIXEMUL := -noixemul
    SYSLIB   := -lc -ldebug -lmath
endif
ifeq ($(HOST_OS),morphos)
    ALIB := -labox -laboxstubs
else
    ALIB := -lamiga
endif
ifeq ($(HOST_CPU),ppc)
    CPU := -mcpu=750
else
    CPU :=
endif

PROJECT=            Scout
TARGET=             $(PROJECT)

VERSION=            37

OUT=                Scout

CC=                 $(PREFIX)gcc
LD=                 $(PREFIX)gcc
STRIP=              $(PREFIX)strip
OBJDUMP=	    $(PREFIX)objdump
RM=                 rm -f

OBJDIR=             .obj
CLASSESDIR=         classes
EXTRASRC=           extrasrc

OPTIMIZE=           -O3
DEFINE=
CFLAGS:=            $(USER_CFLAGS) -Wall $(NOIXEMUL) $(CPU) -I. -idirafter ./include -I./classes -I./tbLib -c
LFLAGS=             $(NOIXEMUL) -nostartfiles

LOCALE=             locale

# $(PROJECT).o _has_ to be the first module !!! (Because we use no
# startup-code)

OBJECTS=$(OBJDIR)/startup.o \
   $(OBJDIR)/Scout.o \
   $(OBJDIR)/scout_logo.o \
   $(OBJDIR)/scout_net.o \
   $(OBJDIR)/$(CLASSESDIR)/fontdisplay_class.o \
   $(OBJDIR)/$(CLASSESDIR)/port_button.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_about.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_allocations.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_arexx.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_asm.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_assigns.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_audiomode.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_audiomode_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_catalogs.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_classes.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_classes_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_commands.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_commodity.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_commodity_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_customclasses.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_devices.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_devices_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_expansions.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_expansions_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_extras.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_flags.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_fonts.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_fonts_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_functions.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_inputhandlers.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_inputhandlers_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_interrupts.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_interrupts_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_libraries.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_libraries_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_locks.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_lowmemory.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_lowmemory_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_main.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_memory.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_memory_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_monitors.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_monitors_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_monitorclass.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_monitorclass_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_mount.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_mount_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_oop.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_oop_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_parentwindow.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_patches.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_ports.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_ports_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_print.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_priority.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_resethandlers.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_resethandlers_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_residents.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_residents_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_resources.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_resources_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_screenmode.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_screenmode_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_semaphores.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_signal.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_system.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_tasks.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_tasks_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_timer.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_vectors.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_version.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_windows.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_windows_detail.o \
   $(OBJDIR)/$(CLASSESDIR)/scout_disassembler.o \
   $(OBJDIR)/$(CLASSESDIR)/disassembler_button.o \
   $(OBJDIR)/$(CLASSESDIR)/flags_button.o \
   $(OBJDIR)/$(CLASSESDIR)/monitor_button.o \
   $(OBJDIR)/$(CLASSESDIR)/task_button.o \
   $(OBJDIR)/scout_cat.o \
   $(OBJDIR)/SmartReadArgs.o \
   $(OBJDIR)/all_gcc.o

HEADERS=system_headers.h startup.h scout.h \
   scout_logo.h scout_net.h \
   scout_defs.h scout_tags.h startup_defs.h \
   $(CLASSESDIR)/fontdisplay_class.h \
   $(CLASSESDIR)/port_button.h \
   $(CLASSESDIR)/scout_about.h \
   $(CLASSESDIR)/scout_allocations.h \
   $(CLASSESDIR)/scout_arexx.h \
   $(CLASSESDIR)/scout_asm.h \
   $(CLASSESDIR)/scout_assigns.h \
   $(CLASSESDIR)/scout_audiomode.h \
   $(CLASSESDIR)/scout_audiomode_detail.h \
   $(CLASSESDIR)/scout_catalogs.h \
   $(CLASSESDIR)/scout_classes.h \
   $(CLASSESDIR)/scout_classes_detail.h \
   $(CLASSESDIR)/scout_commands.h \
   $(CLASSESDIR)/scout_commodity.h \
   $(CLASSESDIR)/scout_commodity_detail.h \
   $(CLASSESDIR)/scout_customclasses.h \
   $(CLASSESDIR)/scout_devices.h \
   $(CLASSESDIR)/scout_devices_detail.h \
   $(CLASSESDIR)/scout_expansions.h \
   $(CLASSESDIR)/scout_expansions_detail.h \
   $(CLASSESDIR)/scout_extras.h \
   $(CLASSESDIR)/scout_flags.h \
   $(CLASSESDIR)/scout_fonts.h \
   $(CLASSESDIR)/scout_fonts_detail.h \
   $(CLASSESDIR)/scout_functions.h \
   $(CLASSESDIR)/scout_inputhandlers.h \
   $(CLASSESDIR)/scout_interrupts.h \
   $(CLASSESDIR)/scout_interrupts_detail.h \
   $(CLASSESDIR)/scout_libraries.h \
   $(CLASSESDIR)/scout_libraries_detail.h \
   $(CLASSESDIR)/scout_locks.h \
   $(CLASSESDIR)/scout_lowmemory.h \
   $(CLASSESDIR)/scout_lowmemory_detail.h \
   $(CLASSESDIR)/scout_main.h \
   $(CLASSESDIR)/scout_memory.h \
   $(CLASSESDIR)/scout_memory_detail.h \
   $(CLASSESDIR)/scout_mount.h \
   $(CLASSESDIR)/scout_mount_detail.h \
   $(CLASSESDIR)/scout_parentwindow.h \
   $(CLASSESDIR)/scout_patches.h \
   $(CLASSESDIR)/scout_ports.h \
   $(CLASSESDIR)/scout_ports_detail.h \
   $(CLASSESDIR)/scout_print.h \
   $(CLASSESDIR)/scout_priority.h \
   $(CLASSESDIR)/scout_resethandlers.h \
   $(CLASSESDIR)/scout_residents.h \
   $(CLASSESDIR)/scout_residents_detail.h \
   $(CLASSESDIR)/scout_resources.h \
   $(CLASSESDIR)/scout_resources_detail.h \
   $(CLASSESDIR)/scout_screenmode.h \
   $(CLASSESDIR)/scout_screenmode_detail.h \
   $(CLASSESDIR)/scout_semaphores.h \
   $(CLASSESDIR)/scout_signal.h \
   $(CLASSESDIR)/scout_system.h \
   $(CLASSESDIR)/scout_tasks.h \
   $(CLASSESDIR)/scout_tasks_detail.h \
   $(CLASSESDIR)/scout_timer.h \
   $(CLASSESDIR)/scout_vectors.h \
   $(CLASSESDIR)/scout_version.h \
   $(CLASSESDIR)/scout_windows.h \
   $(CLASSESDIR)/scout_windows_detail.h \
   $(CLASSESDIR)/scout_disassembler.h \
   $(CLASSESDIR)/disassembler_button.h \
   $(CLASSESDIR)/flags_button.h \
   $(CLASSESDIR)/task_button.h \
   scout_cat.h \
   SmartReadArgs.h \
   include/extra.h

LIBS = -L./netlib -L./tbLib -L./lib $(ALIB) $(SYSLIB) -ltblib

all: $(OBJDIR) $(HEADERS) $(TARGET)

$(OBJDIR):
	@mkdir $(OBJDIR)
	@mkdir $(OBJDIR)/$(CLASSESDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINE) $< -o $@

$(LOCALE)/%.catalog: $(LOCALE)/%.ct
	flexcat $(LOCALE)/scout.cd $< catalog $@ flush

$(TARGET): $(OBJECTS) tbLib/libtblib.a
	$(LD) $(OBJECTS) -o $@.debug $(LIBS) $(LFLAGS)
	$(STRIP) -R.comment -R.sdata2 -S $@.debug -o $@
	chmod +x $@

scout_cat.h: $(LOCALE)/scout.cd
	flexcat locale/scout.cd scout_cat.h=C_h.sd scout_cat.c=C_c.sd

$(OBJDIR)/scout_cat.o: scout_cat.h

$(OBJDIR)/$(CLASSESDIR)/scout_customclasses.o: $(CLASSESDIR)/scout_customclasses.c
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINE) $< -o $@

$(OBJDIR)/$(CLASSESDIR)/scout_about.o: $(CLASSESDIR)/scout_about.c $(CLASSESDIR)/scout_version.h Scout_rev.h
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINE) $< -o $@

$(OBJDIR)/$(CLASSESDIR)/scout_version.o: $(CLASSESDIR)/scout_version.c $(CLASSESDIR)/scout_version.h Scout_rev.h
	$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINE) $< -o $@

$(OBJDIR)/all_gcc.o: all_gcc.c $(EXTRASRC)/init_inet_daemon.c $(EXTRASRC)/vasprintf.c

tbLib/libtblib.a:
	@cd tbLib; make OS=$(OS) USER_CFLAGS=$(USER_CFLAGS)

clean:
	-$(RM) $(OBJDIR)/*.o $(OBJDIR)/$(CLASSESDIR)/*.o $(TARGET) $(TARGET).debug $(TARGET).dump
	@cd tbLib; make $@

realclean: clean

version:
	bumprev2 $(VERSION) $(PROJECT)

date:
	@eval `type $(PROJECT)_rev.rev`-1 >t:$(PROJECT)_rev.rev
	@copy t:$(PROJECT)_rev.rev $(PROJECT)_rev.rev
	@delete >nil: t:$(PROJECT)_rev.rev
	@smake version

dump:
	$(OBJDUMP) --section-headers --all-headers --reloc --syms --disassemble-all $(TARGET).debug >$(TARGET).dump

install: $(PROJECT)
	copy $(PROJECT) Work:Debug/Scout

cvs-tag:
	cvs -q tag V$(VERSION)_`type $(PROJECT)_rev.rev`

cvs-update:
	cvs update

doc:
	@execute <<
	pushcd help
	smake
	popcd

