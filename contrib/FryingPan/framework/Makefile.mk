#*** Section 1: defaults for most projects

# set these ONLY IN YOUR OWN MAKEFILES

# Variable: INSTALLDIR
# set the directory where target will be copied
INSTALLDIR			?= 

# Variable: Program
# define program name
Program				?= main

# Variable: OBJS
# define list of objects to build
OBJS					?= main.o

# Variable: BUILD_LIBRARY
# select, whether to build object lib or executable file
BUILD_LIBRARY		?= 0

# Variable: OWNSTARTUP
# define startup type:
# 0: use default startup
# 1: use private startup
# 2: private, no wb startup
OWNSTARTUP			?= 1

# Variable: TARGETS
# define targets for which the build will be made. Currently supported targets:
# - aros
# - os3 
# - morphos
# - os4
#   linux targets are NOT supported
TARGETS				?= aros os3 morphos os4

# Variable: LIBOPTS
# define link options for linking
LIBOPTS				?= -lGeneric -lC -nostdlib

# Variable: WSPATH
# workspace directory where your Generic setup lays.
WSPATH            ?= $(HOME)/workspace

# Variable: INCOPTS
# include options for compilation
INCOPTS				?= 

# Variable: OBJDIRS
# list of object directories to create. Typically empty.
OBJDIRS				?= 

# Variable: MAKEDIRS
# list of directories for which you want to invoke make. typically empty.
MAKEDIRS				?= 

# Variable: PREP
# preprocessor directives. typically empty
PREP					?= 

# Variable: BINPATH
# Set this to the directory where your compilers are all set
BINPATH		  ?= /usr/local/amiga/bin

#---

#*** Section 2: handles the binaries & paths 

AROSBIN 		= bin-aros
M68KBIN 		= bin-os3
MORPHOSBIN 	= bin-mos
OS4BIN 		= bin-os4
LINUXBIN		= bin-linux
RELDIR		= release
DBGDIR		= debug

AROSRBIN 	= $(AROSBIN)/$(RELDIR)
M68KRBIN 	= $(M68KBIN)/$(RELDIR)
MORPHOSRBIN = $(MORPHOSBIN)/$(RELDIR)
OS4RBIN 		= $(OS4BIN)/$(RELDIR)
LINUXRBIN	= $(LINUXBIN)/$(RELDIR)

AROSDBIN 	= $(AROSBIN)/$(DBGDIR)
M68KDBIN 	= $(M68KBIN)/$(DBGDIR)
MORPHOSDBIN = $(MORPHOSBIN)/$(DBGDIR)
OS4DBIN 		= $(OS4BIN)/$(DBGDIR)
LINUXDBIN	= $(LINUXBIN)/$(DBGDIR)


AROSDOBJ 	= $(AROSDBIN)/obj/
M68KDOBJ 	= $(M68KDBIN)/obj/
MORPHOSDOBJ = $(MORPHOSDBIN)/obj/
OS4DOBJ 		= $(OS4DBIN)/obj/
LINUXDOBJ	= $(LINUXDBIN)/obj/

AROSROBJ 	= $(AROSRBIN)/obj/
M68KROBJ 	= $(M68KRBIN)/obj/
MORPHOSROBJ = $(MORPHOSRBIN)/obj/
OS4ROBJ 		= $(OS4RBIN)/obj/
LINUXROBJ	= $(LINUXRBIN)/obj/


AROSR 		= $(addprefix $(AROSROBJ), $(OBJS))
M68KR 		= $(addprefix $(M68KROBJ), $(OBJS))
MORPHOSR		= $(addprefix $(MORPHOSROBJ), $(OBJS))
OS4R 			= $(addprefix $(OS4ROBJ), $(OBJS))
LINUXR		= $(addprefix $(LINUXROBJ), $(OBJS))

AROSD 		= $(addprefix $(AROSDOBJ), $(OBJS))
M68KD 		= $(addprefix $(M68KDOBJ), $(OBJS))
MORPHOSD		= $(addprefix $(MORPHOSDOBJ), $(OBJS))
OS4D 			= $(addprefix $(OS4DOBJ), $(OBJS))
LINUXD		= $(addprefix $(LINUXDOBJ), $(OBJS))


DIRS			=	$(AROSRBIN) $(M68KRBIN) $(MORPHOSRBIN) $(OS4RBIN) $(LINUXRBIN) \
					$(AROSDBIN) $(M68KDBIN) $(MORPHOSDBIN) $(OS4DBIN) $(LINUXDBIN) \
					$(AROSROBJ) $(M68KROBJ) $(MORPHOSROBJ) $(OS4ROBJ) $(LINUXROBJ) \
					$(AROSDOBJ) $(M68KDOBJ) $(MORPHOSDOBJ) $(OS4DOBJ) $(LINUXDOBJ) \
					$(addprefix $(AROSROBJ), $(OBJDIRS))		\
					$(addprefix $(M68KROBJ), $(OBJDIRS))		\
					$(addprefix $(MORPHOSROBJ), $(OBJDIRS))	\
					$(addprefix $(OS4ROBJ), $(OBJDIRS))			\
					$(addprefix $(LINUXROBJ), $(OBJDIRS))		\
					$(addprefix $(AROSDOBJ), $(OBJDIRS))		\
					$(addprefix $(M68KDOBJ), $(OBJDIRS))		\
					$(addprefix $(MORPHOSDOBJ), $(OBJDIRS))	\
					$(addprefix $(OS4DOBJ), $(OBJDIRS))			\
					$(addprefix $(LINUXDOBJ), $(OBJDIRS))			

ifdef ($(INSTALLDIR))
DIRS 			+= $(INSTALLDIR)/$(AROSRBIN) \
					$(INSTALLDIR)/$(AROSDBIN) \
					$(INSTALLDIR)/$(M68KRBIN) \
					$(INSTALLDIR)/$(M68KDBIN) \
					$(INSTALLDIR)/$(MORPHOSRBIN) \
					$(INSTALLDIR)/$(MORPHOSDBIN) \
					$(INSTALLDIR)/$(OS4RBIN) \
					$(INSTALLDIR)/$(OS4DBIN)
endif

#---

#*** Section 3: Compilers, linkers, archivers, you don't want to change anything

GENLIB			= $(WSPATH)/Generic/lib
GENINC        += $(INCOPTS) -I$(WSPATH)/Generic -I$(WSPATH)

M68K_CC 			= $(BINPATH)/m68k-amigaos-gcc 
AROS_CC			= $(BINPATH)/i686-aros-g++
MORPHOS_CC 		= $(BINPATH)/ppc-morphos-gcc
OS4_CC			= $(BINPATH)/ppc-amigaos-g++
LINUX_CC			= /usr/bin/g++

M68K_AR 			= $(BINPATH)/m68k-amigaos-ar
AROS_AR			= $(BINPATH)/i686-aros-ar
MORPHOS_AR 		= $(BINPATH)/ppc-morphos-ar
OS4_AR			= $(BINPATH)/ppc-amigaos-ar
LINUX_AR			= /usr/bin/ar

M68K_STRIP		= $(BINPATH)/m68k-amigaos-strip
AROS_STRIP		= $(BINPATH)/i686-aros-strip
MORPHOS_STRIP	= $(BINPATH)/ppc-morphos-strip
OS4_STRIP		= $(BINPATH)/ppc-amigaos-strip
LINUX_STRIP		= /usr/bin/strip


AROS_RLIBOPTS	+= -L$(GENLIB)/$(AROSRBIN)
MOS_RLIBOPTS	+= -L$(GENLIB)/$(MORPHOSRBIN)
M68K_RLIBOPTS	+= -L$(GENLIB)/$(M68KRBIN)
OS4_RLIBOPTS	+= -L$(GENLIB)/$(OS4RBIN)

AROS_DLIBOPTS  += -L$(GENLIB)/$(AROSDBIN)
MOS_DLIBOPTS	+= -L$(GENLIB)/$(MORPHOSDBIN)
M68K_DLIBOPTS  += -L$(GENLIB)/$(M68KDBIN)
OS4_DLIBOPTS	+= -L$(GENLIB)/$(OS4DBIN)

ifeq ($(OWNSTARTUP),1)
AROS_RSTARTUP  += $(GENLIB)/$(AROSRBIN)/Startup.o
MOS_RSTARTUP	+= $(GENLIB)/$(MORPHOSRBIN)/Startup.o
M68K_RSTARTUP  += $(GENLIB)/$(M68KRBIN)/Startup.o
OS4_RSTARTUP	+= $(GENLIB)/$(OS4RBIN)/Startup.o
AROS_DSTARTUP  += $(GENLIB)/$(AROSDBIN)/Startup.o
MOS_DSTARTUP	+= $(GENLIB)/$(MORPHOSDBIN)/Startup.o
M68K_DSTARTUP  += $(GENLIB)/$(M68KDBIN)/Startup.o
OS4_DSTARTUP	+= $(GENLIB)/$(OS4DBIN)/Startup.o
LIBOPTS		  	+= -nodefaultlibs -nostdlib -nostartfiles 
endif

ifeq ($(OWNSTARTUP),2)
AROS_RSTARTUP  += $(GENLIB)/$(AROSRBIN)/NoWBStartup.o
MOS_RSTARTUP	+= $(GENLIB)/$(MORPHOSRBIN)/NoWBStartup.o
M68K_RSTARTUP  += $(GENLIB)/$(M68KRBIN)/NoWBStartup.o
OS4_RSTARTUP	+= $(GENLIB)/$(OS4RBIN)/NoWBStartup.o
AROS_DSTARTUP  += $(GENLIB)/$(AROSDBIN)/NoWBStartup.o
MOS_DSTARTUP	+= $(GENLIB)/$(MORPHOSDBIN)/NoWBStartup.o
M68K_DSTARTUP  += $(GENLIB)/$(M68KDBIN)/NoWBStartup.o
OS4_DSTARTUP	+= $(GENLIB)/$(OS4DBIN)/NoWBStartup.o
LIBOPTS		  	+= -nodefaultlibs -nostdlib -nostartfiles 
endif

#---

#*** Section 4: Compiler flags
P2CCFLAGS          = -mcpu=604e -O2 -fomit-frame-pointer -mmultiple

CPPFLAGS				+= -fno-rtti -fno-exceptions -fno-check-new
CCFLAGS				+= -Wall -fno-stack-check -fno-builtin -Wno-multichar -Wno-inline -finline -finline-functions
M68K_RCCFLAGS		+= -D__BIG_ENDIAN__ -fno-baserel -noixemul -I/opt/gg/sysinc -m68030 -m68881 $(GENINC) $(CCFLAGS) -O3 -Wuninitialized -fomit-frame-pointer
AROS_RCCFLAGS		+= -D__LITTLE_ENDIAN__ -static -I/usr/local/i686-aros/sys-include $(GENINC) $(CCFLAGS) -Wuninitialized -O3
MORPHOS_RCCFLAGS 	+= -D__BIG_ENDIAN__ -noixemul -I/opt/gg/mos-include $(GENINC) $(CCFLAGS) -Wuninitialized -O3 #$(P2CCFLAGS)  #-O3
OS4_RCCFLAGS		+= -D__BIG_ENDIAN__ -noixemul -D __USE_INLINE__ -fno-enforce-eh-specs -I/SDK/Include/include_h -I/SDK/include $(GENINC) $(CCFLAGS) -Wuninitialized -O3
LINUX_RCCFLAGS		+= $(CCFLAGS)

M68K_DCCFLAGS		+= -D__BIG_ENDIAN__ -fno-baserel -noixemul -I/opt/gg/sysinc -m68030 -m68881 $(GENINC) $(CCFLAGS) -DDBG -DDEBUG -g
AROS_DCCFLAGS		+= -D__LITTLE_ENDIAN__ -static -I/usr/local/i686-aros/sys-include $(GENINC) $(CCFLAGS) -DDBG -DDEBUG -g
MORPHOS_DCCFLAGS 	+= -D__BIG_ENDIAN__ -noixemul -I/opt/gg/mos-include $(GENINC) $(CCFLAGS) -DDBG -DDEBUG -g
OS4_DCCFLAGS		+= -D__BIG_ENDIAN__ -noixemul -D __USE_INLINE__ -fno-enforce-eh-specs -I/SDK/Include/include_h -I/SDK/include $(GENINC) $(CCFLAGS) -DDBG -DDEBUG -g
LINUX_DCCFLAGS		+= $(CCFLAGS)

#---

#*** Section 5: Build specific settings

ifeq ($(BUILD_LIBRARY), 0)
	AROS_RLINK	= $(AROS_CC) $(AROS_RCCFLAGS) -o $(AROSRBIN)/$(Program) $(AROS_RSTARTUP) $(AROSR) $(AROS_RLIBOPTS) $(LIBOPTS) 
	M68K_RLINK	= $(M68K_CC) $(M68K_RCCFLAGS) -o $(M68KRBIN)/$(Program) $(M68K_RSTARTUP) $(M68KR) $(M68K_RLIBOPTS) $(LIBOPTS)
	MOS_RLINK	= $(MORPHOS_CC) $(MORPHOS_RCCFLAGS) -o $(MORPHOSRBIN)/$(Program) $(MOS_RSTARTUP) $(MORPHOSR) $(MOS_RLIBOPTS) $(LIBOPTS)
	OS4_RLINK	= $(OS4_CC) $(OS4_RCCFLAGS) -o $(OS4RBIN)/$(Program) $(OS4_RSTARTUP) $(OS4R) $(OS4_RLIBOPTS) $(LIBOPTS)
	LINUX_RLINK	= $(LINUX_CC) $(LINUX_RCCFLAGS) -o $(LINUXRBIN)/$(Program) $(LINUX_RSTARTUP) $(LINUXR) $(LINUX_RLIBOPTS) $(LIBOPTS) 

	AROS_RSTRIP = $(AROS_STRIP) -s $(AROSRBIN)/$(Program)
	MOS_RSTRIP 	= $(MORPHOS_STRIP) -s $(MORPHOSRBIN)/$(Program)
	OS4_RSTRIP 	= $(OS4_STRIP) -s $(OS4RBIN)/$(Program)

	AROS_DLINK	= $(AROS_CC) $(AROS_DCCFLAGS) -o $(AROSDBIN)/$(Program) $(AROS_DSTARTUP) $(AROSD) $(AROS_DLIBOPTS) $(LIBOPTS) 
	M68K_DLINK	= $(M68K_CC) $(M68K_DCCFLAGS) -o $(M68KDBIN)/$(Program) $(M68K_DSTARTUP) $(M68KD) $(M68K_DLIBOPTS) $(LIBOPTS)
	MOS_DLINK	= $(MORPHOS_CC) $(MORPHOS_DCCFLAGS) -o $(MORPHOSDBIN)/$(Program) $(MOS_DSTARTUP) $(MORPHOSD) $(MOS_DLIBOPTS) $(LIBOPTS)
	OS4_DLINK	= $(OS4_CC) $(OS4_DCCFLAGS) -o $(OS4DBIN)/$(Program) $(OS4_DSTARTUP) $(OS4D) $(OS4_DLIBOPTS) $(LIBOPTS)
	LINUX_DLINK	= $(LINUX_CC) $(LINUX_DCCFLAGS) -o $(LINUXDBIN)/$(Program) $(LINUX_DSTARTUP) $(LINUXD) $(LINUX_DLIBOPTS) $(LIBOPTS) 
else
	AROS_RLINK	= $(AROS_AR) r $(AROSRBIN)/$(Program) $(AROSR) 
	M68K_RLINK	= $(M68K_AR) r $(M68KRBIN)/$(Program) $(M68KR)
	MOS_RLINK	= $(MORPHOS_AR) r $(MORPHOSRBIN)/$(Program) $(MORPHOSR)
	OS4_RLINK	= $(OS4_AR) r $(OS4RBIN)/$(Program) $(OS4R)
	LINUX_RLINK	= $(LINUX_AR) r $(LINUXRBIN)/$(Program) $(LINUXR)

	AROS_RSTRIP = $(AROS_STRIP) --strip-debug $(AROSRBIN)/$(Program)
	MOS_RSTRIP 	= $(MORPHOS_STRIP) --strip-debug $(MORPHOSRBIN)/$(Program)
	OS4_RSTRIP 	= $(OS4_STRIP) --strip-debug $(OS4RBIN)/$(Program)

	AROS_DLINK	= $(AROS_AR) r $(AROSDBIN)/$(Program) $(AROSD) 
	M68K_DLINK	= $(M68K_AR) r $(M68KDBIN)/$(Program) $(M68KD)
	MOS_DLINK	= $(MORPHOS_AR) r $(MORPHOSDBIN)/$(Program) $(MORPHOSD)
	OS4_DLINK	= $(OS4_AR) r $(OS4DBIN)/$(Program) $(OS4D)
	LINUX_DLINK	= $(LINUX_AR) r $(LINUXDBIN)/$(Program) $(LINUXD)
endif

#---

#*** Section 6: Rules.


all: $(DIRS) submakes $(TARGETS)
	@echo "\t[  INFO] All done."
	
clean: subcleans 
	@echo "\t[  INFO] Cleaning up directories..."
	@rm -rf $(AROSBIN)
	@rm -rf $(M68KBIN)
	@rm -rf $(MORPHOSBIN)
	@rm -rf $(OS4BIN)
	@rm -rf $(LINUXBIN)
	@find . -name "*~" -exec rm '{}' \;
	@find . -name ".*.swp" -exec rm '{}' \;
	@echo "\t[  INFO] Cleaned up."
	
distclean: clean
	@echo "\t[  INFO] Cleaning up other data..."
	@find . -name "cscope.*" -exec rm '{}' \;
	@echo "\t[  INFO] All done."

$(DIRS):
	@echo "\t[  INFO] Creating directory $@"
	@mkdir -p $@
	
submakes:
	@echo "\t[  SUB ] Making sub-elements... "
	@$(foreach subdir, $(MAKEDIRS), $(MAKE) -C $(subdir);)

subcleans:
	@echo "\t[  SUB ] Cleaning sub-elements... "
	$(foreach subdir, $(MAKEDIRS), $(MAKE) -C $(subdir) clean;)
	
aros: $(AROSR) $(AROSD)
	@echo "\t[  AROS] Linking... "
ifeq ($(NOLINK),1)
	@cp $(AROSROBJ)/$(Program) $(AROSRBIN)
	@cp $(AROSDOBJ)/$(Program) $(AROSDBIN)
else
	@$(AROS_RLINK)
	@$(AROS_DLINK)
endif
ifdef INSTALLDIR
	@cp $(AROSRBIN)/$(Program) $(INSTALLDIR)/$(AROSRBIN)/$(Program)
	@cp $(AROSDBIN)/$(Program) $(INSTALLDIR)/$(AROSDBIN)/$(Program)
endif

os3: $(M68KR) $(M68KD)
	@echo "\t[  M68K] Linking... "
ifeq ($(NOLINK),1)
	@cp $(M68KROBJ)/$(Program) $(M68KRBIN)
	@cp $(M68KDOBJ)/$(Program) $(M68KDBIN)
else
	@$(M68K_RLINK)
	@$(M68K_DLINK)
endif
ifdef INSTALLDIR
	@cp $(M68KRBIN)/$(Program) $(INSTALLDIR)/$(M68KRBIN)/$(Program)
	@cp $(M68KDBIN)/$(Program) $(INSTALLDIR)/$(M68KDBIN)/$(Program)
endif

morphos: $(MORPHOSR) $(MORPHOSD)
	@echo "\t[  MOS ] Linking... "
ifeq ($(NOLINK),1)
	@cp $(MORPHOSROBJ)/$(Program) $(MORPHOSRBIN)
	@cp $(MORPHOSDOBJ)/$(Program) $(MORPHOSDBIN)
else
	@$(MOS_RLINK)
	@$(MOS_DLINK)
endif
ifdef INSTALLDIR
	@cp $(MORPHOSRBIN)/$(Program) $(INSTALLDIR)/$(MORPHOSRBIN)/$(Program)
	@cp $(MORPHOSDBIN)/$(Program) $(INSTALLDIR)/$(MORPHOSDBIN)/$(Program)
endif

os4: $(OS4R) $(OS4D)
	@echo "\t[  OS4 ] Linking... "
ifeq ($(NOLINK),1)
	@cp $(OS4ROBJ)/$(Program) $(OS4RBIN)
	@cp $(OS4DOBJ)/$(Program) $(OS4DBIN)
else
	@$(OS4_RLINK)
	@$(OS4_DLINK)
endif
ifdef INSTALLDIR
	@cp $(OS4RBIN)/$(Program) $(INSTALLDIR)/$(OS4RBIN)/$(Program)
	@cp $(OS4DBIN)/$(Program) $(INSTALLDIR)/$(OS4DBIN)/$(Program)
endif

linux: $(LINUXR) $(LINUXD)
	@echo "\t[  LIN ] Linking... "
ifeq ($(NOLINK),1)
	@cp $(LINUXROBJ)/$(Program) $(LINUXRBIN)
	@cp $(LINUXDOBJ)/$(Program) $(LINUXDBIN)
else
	@$(LINUX_RLINK)
	@$(LINUX_DLINK)
endif
ifdef INSTALLDIR
	@cp $(LINUXRBIN)/$(Program) $(INSTALLDIR)/$(LINUXRBIN)/$(Program)
	@cp $(LINUXDBIN)/$(Program) $(INSTALLDIR)/$(LINUXDBIN)/$(Program)
endif
#---

#*** Platform specific targets?

$(AROSROBJ)%.o:	%_aros.cpp $(DEPS)
	@echo "\t[R AROS] CC $* "
	@$(AROS_CC) $(AROS_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_aros.cpp

$(M68KROBJ)%.o:	%_os3.cpp $(DEPS)
	@echo "\t[R M68K] CC $* "
	@$(M68K_CC) $(M68K_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_os3.cpp
	
$(MORPHOSROBJ)%.o: %_mos.cpp $(DEPS)
	@echo "\t[R MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_mos.cpp

$(OS4ROBJ)%.o: %_os4.cpp $(DEPS)
	@echo "\t[R OS4 ] CC $* "
	@$(OS4_CC) $(OS4_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_os4.cpp
	
$(LINUXROBJ)%.o: %_linux.cpp $(DEPS)
	@echo "\t[R LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_linux.cpp
	
$(AROSDOBJ)%.o:	%_aros.cpp $(DEPS)
	@echo "\t[D AROS] CC $* "
	@$(AROS_CC) $(AROS_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_aros.cpp

$(M68KDOBJ)%.o:	%_os3.cpp $(DEPS)
	@echo "\t[D M68K] CC $* "
	@$(M68K_CC) $(M68K_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_os3.cpp
	
$(MORPHOSDOBJ)%.o: %_mos.cpp $(DEPS)
	@echo "\t[D MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_mos.cpp

$(OS4DOBJ)%.o: %_os4.cpp $(DEPS)
	@echo "\t[D OS4 ] CC $* "
	@$(OS4_CC) $(OS4_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_os4.cpp
	
$(LINUXDOBJ)%.o: %_linux.cpp $(DEPS)
	@echo "\t[D LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*_linux.cpp
#---

#*** Global targets.

$(AROSROBJ)%.o: %.cpp $(DEPS)
	@echo "\t[R AROS] CC $* "
	@$(AROS_CC) $(AROS_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(M68KROBJ)%.o: %.cpp $(DEPS)
	@echo "\t[R M68K] CC $* "
	@$(M68K_CC) $(M68K_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp
	
$(MORPHOSROBJ)%.o: %.cpp $(DEPS)
	@echo "\t[R MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(OS4ROBJ)%.o: %.cpp $(DEPS)
	@echo "\t[R OS4 ] CC $* "
	@$(OS4_CC) $(OS4_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(LINUXROBJ)%.o: %.cpp $(DEPS)
	@echo "\t[R LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_RCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(AROSDOBJ)%.o: %.cpp $(DEPS)
	@echo "\t[D AROS] CC $* "
	@$(AROS_CC) $(AROS_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(M68KDOBJ)%.o: %.cpp $(DEPS)
	@echo "\t[D M68K] CC $* "
	@$(M68K_CC) $(M68K_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp
	
$(MORPHOSDOBJ)%.o: %.cpp $(DEPS)
	@echo "\t[D MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(OS4DOBJ)%.o: %.cpp $(DEPS)
	@echo "\t[D OS4 ] CC $* "
	@$(OS4_CC) $(OS4_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

$(LINUXDOBJ)%.o: %.cpp $(DEPS)
	@echo "\t[D LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_DCCFLAGS) $(CPPFLAGS) $(PREP) -o $@ -c $*.cpp

#---	

#*** C targets.

$(AROSROBJ)%.o: %.c $(DEPS)
	@echo "\t[R AROS] CC $* "
	@$(AROS_CC) $(AROS_RCCFLAGS) $(PREP) -o $@ -c $*.c

$(M68KROBJ)%.o: %.c $(DEPS)
	@echo "\t[R M68K] CC $* "
	@$(M68K_CC) $(M68K_RCCFLAGS) $(PREP) -o $@ -c $*.c
	
$(MORPHOSROBJ)%.o: %.c $(DEPS)
	@echo "\t[R MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_RCCFLAGS) $(PREP) -o $@ -c $*.c

$(OS4ROBJ)%.o: %.c $(DEPS)
	@echo "\t[R OS4 ] CC $* "
	@$(OS4_CC) $(OS4_RCCFLAGS) $(PREP) -o $@ -c $*.c

$(LINUXROBJ)%.o: %.c $(DEPS)
	@echo "\t[R LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_RCCFLAGS) $(PREP) -o $@ -c $*.c

$(AROSDOBJ)%.o: %.c $(DEPS)
	@echo "\t[D AROS] CC $* "
	@$(AROS_CC) $(AROS_DCCFLAGS) $(PREP) -o $@ -c $*.c

$(M68KDOBJ)%.o: %.c $(DEPS)
	@echo "\t[D M68K] CC $* "
	@$(M68K_CC) $(M68K_DCCFLAGS) $(PREP) -o $@ -c $*.c
	
$(MORPHOSDOBJ)%.o: %.c $(DEPS)
	@echo "\t[D MOS ] CC $* "
	@$(MORPHOS_CC) $(MORPHOS_DCCFLAGS) $(PREP) -o $@ -c $*.c

$(OS4DOBJ)%.o: %.c $(DEPS)
	@echo "\t[D OS4 ] CC $* "
	@$(OS4_CC) $(OS4_DCCFLAGS) $(PREP) -o $@ -c $*.c

$(LINUXDOBJ)%.o: %.c $(DEPS)
	@echo "\t[D LIN ] CC $* "
	@$(LINUX_CC) $(LINUX_DCCFLAGS) $(PREP) -o $@ -c $*.c

#---

# vim: ts=3
