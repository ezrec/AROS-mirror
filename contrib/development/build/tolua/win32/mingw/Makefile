# =============================================================================
#
# MinGW/msys makefile for tolua++
# PUBLIC DOMAIN
# author: Kein-Hong Man, khman@users.sf.net, 20070103
#
# =============================================================================
#
# VERSION  tolua++ 1.0.92
# REQUIRES mingw 5.0.2 + msys 1.0.10
# REQUIRES lua 5.1.1 (official sources), roughly, the following files:
# 	   * ./src/lua.h, ./src/lualib.h, ./src/lauxlib.h
#	   * ./src/liblua.a ("make generic" to get static library)
#	   * ./src/lua51.dll ("make mingw" to get DLL)
#`	   * lua and tolua++ folders should be in the same parent directory
#
# =============================================================================
#
# INSTRUCTIONS
# * see that VERSION and REQUIRES are fulfilled, if not, some adjustments
#   might be necessary, feedback is welcome
# * unpack the tolua++ sources, start at ./win32/mingw
# * look at all FIXME items and adjust accordingly
# * then "make" will build both dynamic and static versions, dumping all
#   intermediate and final files in this directory
# * "make test" builds some test programs, nothing much at present
# * "make install" copies the static executable, header file, and library
#   file to the approriate places
#
# BOOTSTRAPPING
# * to rebuild toluabind.c and toluabind.h, you need a copy of tolua++,
#   so first bootstrap by uncommenting "TOLUABIND_USE= toluabind_default"
#   in the directives below
# * this builds a copy of tolua++.exe, next comment out the bootstrapping
#   setting and try "make" again to generate toluabind.c and toluabind.h
#
# NOTE
# * this Makefile doesn't deal with all source dependencies properly
#
# =============================================================================

# FIXME adjust LUA_PATH for getting some Lua files
LUA_PATH= ../../../lua-5.1.1/src

# FIXME for Lua 5.1.1, this should be in LUA_PATH
LUA_DLL= lua51.dll
LUA_LIB= liblua.a

# the following should be correct unless you move this Makefile elsewhere
LIB_PATH= ../../src/lib
APP_PATH= ../../src/bin
TOL_PATH=../../src/bin/lua
TST_PATH= ../../src/tests
INC_PATH= ../../include
# only for rebuilding
EXE_PATH= win32/mingw
SRC_PATH= src/bin

vpath %.h $(LIB_PATH) $(APP_PATH) $(TST_PATH) $(INC_PATH) $(LUA_PATH)
vpath %.c $(LIB_PATH) $(APP_PATH) $(TST_PATH)
vpath %.pkg $(TST_PATH)
vpath %.lua $(TOL_PATH)

# install directories
INSTALL_TOP= /usr
INSTALL_BIN= $(INSTALL_TOP)/bin
INSTALL_INC= $(INSTALL_TOP)/include
INSTALL_LIB= $(INSTALL_TOP)/lib

# =============================================================================

CP= cp
FIND= find
INSTALL= install
MKDIR= mkdir
RANLIB= ranlib

# =============================================================================

H_PATH= -I$(INC_PATH) -I$(LUA_PATH) -I$(TST_PATH)

CC= gcc
CXX= g++
CFLAGS= -O2 -Wall $(H_PATH) $(MYCFLAGS)
CXXFLAGS= -O2 -Wall $(H_PATH) $(MYCXXFLAGS)

# dynamic library
AR_D= $(CC) -shared -o
RANLIB_D= strip --strip-unneeded

# static library
AR_S= ar rcu
RANLIB_S= ranlib

RM= rm -f
LIBS= -lm $(MYLIBS)

# FIXME put your custom compilation flags here
MYCFLAGS=
MYCXXFLAGS=
MYLDFLAGS=
MYLIBS=

# =============================================================================

PROGRAM= tolua++

# program names, dynamic and static
APP= $(PROGRAM).exe
APS= $(PROGRAM)-s.exe

# library names, dynamic and static
LIB_D= $(PROGRAM).dll
LIB_S= lib$(PROGRAM).a

# name of embedded Lua code file
TOLUABIND= toluabind

# FIXME choose one
# build_dev: Build for development (uses tolua to rebuild toluabind.c with the embedded scripts)
TOLUABIND_USE= $(TOLUABIND)
# build_failsafe: Build using 'factory default' toluabind file (in case build_dev fails)
#TOLUABIND_USE= toluabind_default

# =============================================================================

LIB_O=	tolua_event.o \
	tolua_is.o \
	tolua_map.o \
	tolua_push.o \
	tolua_to.o

APP_O=	tolua.o \
	$(TOLUABIND_USE).o

LUA_SRC= \
	compat-5.1.lua \
	compat.lua \
	basic.lua \
	feature.lua \
	verbatim.lua \
	code.lua \
	typedef.lua \
	container.lua \
	package.lua \
	module.lua \
	namespace.lua \
	define.lua \
	enumerate.lua \
	declaration.lua \
	variable.lua \
	array.lua \
	function.lua \
	operator.lua \
	class.lua \
	clean.lua \
	doit.lua

# =============================================================================

all:	$(APP) $(APS) $(LIB_D) $(LIB_S)

# copy over Lua DLL
$(LUA_DLL): $(LUA_PATH)/$(LUA_DLL)
	$(CP) $< $@

# rebuild toluabind.c
# NOTE tolua++ must be evoked from the package root directory
$(APP_PATH)/$(TOLUABIND).c: $(APP_PATH)/tolua_scons.pkg $(LUA_SRC)
	cd ../.. && $(EXE_PATH)/$(APP) \
	   -n tolua \
	   -H $(SRC_PATH)/$(TOLUABIND).h \
	   -o $(SRC_PATH)/$(TOLUABIND).c \
	   $(SRC_PATH)/tolua_scons.pkg

# build main executable, dynamically linked
$(APP):	$(APP_O) $(LUA_DLL) $(LIB_D)
	$(CC) -o $@ -s $(MYLDFLAGS) $^

# build main executable, statically linked
$(APS):	$(APP_O) $(LUA_PATH)/$(LUA_LIB) $(LIB_S)
	$(CC) -o $@ $(MYLDFLAGS) $^

# library, dynamically linked
$(LIB_D): $(LIB_O) $(LUA_DLL)
	$(AR_D) $@ $?
	$(RANLIB_D) $@

# library, statically linked
$(LIB_S): $(LIB_O) $(LUA_PATH)/$(LUA_LIB)
	$(AR_S) $@ $?
	$(RANLIB_S) $@

# =============================================================================

# FIXME TODO this portion is under heavy construction... don't bother with this
# unless you are prepared to hack at the test sources

TARRAY= tarray
TVARIA= tvariable
#TCONST= tconstant
TESTS=	$(TARRAY).exe \
	$(TVARIA).exe
#	$(TCONST).exe

test:	$(TESTS)

# FIXME TODO my mingw-compiled version of tolua++ insists on -n, why?

$(TARRAY).lua: $(TST_PATH)/$(TARRAY).lua
	$(CP) $< $@
$(TARRAY)bind.c: $(TARRAY).pkg
	$(APP) -n $(TARRAY) -o $@ $<
$(TARRAY).exe: $(TARRAY).o $(TARRAY)bind.o $(LUA_DLL) $(LIB_D) $(TARRAY).lua
	$(CC) -o $@ -s $(MYLDFLAGS) $(TARRAY).o $(TARRAY)bind.o $(LUA_DLL) $(LIB_D)

# FIXME TODO tvariable fails on assign to n, assert wants table instead of string

$(TVARIA).lua: $(TST_PATH)/$(TVARIA).lua
	$(CP) $< $@
$(TVARIA)bind.c: $(TVARIA).pkg
	$(APP) -n $(TVARIA) -o $@ $<
$(TVARIA).exe: $(TVARIA).o $(TVARIA)bind.o $(LUA_DLL) $(LIB_D) $(TVARIA).lua
	$(CC) -o $@ -s $(MYLDFLAGS) $(TVARIA).o $(TVARIA)bind.o $(LUA_DLL) $(LIB_D)

# FIXME TODO C++ tests tconstant needs a main program
#$(TCONST).lua: $(TST_PATH)/$(TCONST).lua
#	$(CP) $< $@
#$(TCONST)bind.cc: $(TCONST).pkg
#	$(APP) -n $(TCONST) -o $@ $<
#$(TCONST).exe: $(TCONST).o $(TCONST)bind.o $(LUA_DLL) $(LIB_D) $(TCONST).lua
#	$(CXX) -o $@ -s $(MYLDFLAGS) $(TCONST).o $(TCONST)bind.o $(LUA_DLL) $(LIB_D)

zap:
	$(RM) $(TARRAY)bind.* $(TARRAY).*
	$(RM) $(TVARIA)bind.* $(TVARIA).*
#	$(RM) $(TCONST)bind.* $(TCONST).*

# =============================================================================

# FIXME this "installs" the static version of tolua++ (less DLLs to track...)
# * "install" isn't used because, sadly, I need to read up on it first...
# * the following is for copying over the dynamically-linked version of tolua++
#	$(CP) $(APP) $(INSTALL_BIN)
#	$(CP) $(LUA_DLL) $(INSTALL_BIN)
#	$(CP) $(LIB_D) $(INSTALL_BIN)
install: $(APS) $(LIB_D) $(LIB_S)
	@echo ---
	@echo --- Copying files to destination locations...
	@echo ---
	$(CP) $(APS) $(INSTALL_BIN)/$(APP)
	$(CP) $(LIB_D) $(INSTALL_LIB)
	$(CP) $(LIB_S) $(INSTALL_LIB)
	$(CP) $(INC_PATH)/*.h $(INSTALL_INC)
	@echo ---
	@echo --- Done!
	@echo --- Remember to copy the appropriate DLLs, e.g. $(LIB_D), over
	@echo --- to your executable directory if you use dynamic linking!
	@echo ---

# =============================================================================

clean:	zap
	$(RM) $(APP) $(APS) $(LIB_D) $(LIB_S) *.o $(LUA_DLL)

echo:
	@echo "PROGRAM = $(PROGRAM)"
	@echo "CC = $(CC)"
	@echo "CXX = $(CXX)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	@echo "AR_S = $(AR_S)"
	@echo "RANLIB_S = $(RANLIB_S)"
	@echo "AR_D = $(AR_D)"
	@echo "RANLIB_D = $(RANLIB_D)"
	@echo "RM = $(RM)"
	@echo "MYCFLAGS = $(MYCFLAGS)"
	@echo "MYCXXFLAGS = $(MYCXXFLAGS)"
	@echo "MYLDFLAGS = $(MYLDFLAGS)"
	@echo "MYLIBS = $(MYLIBS)"

.PHONY: all test zap install clean echo

# =============================================================================

# (end of Makefile)
