############################################################
CXX := CC
STRIP := strip
############################################################
MKDIR_CMD = test -d $(@D) || mkdir -p $(@D)
############################################################

TARGET_DIR := $(TARGET).app/Contents
TARGET := $(TARGET_DIR)/MacOS/4p

SRCS += \
	src/archs/darwin/Darwin-main.m \
	src/archs/darwin/Darwin-main-help.cpp

OBJS += \
	src/archs/darwin/Darwin-main.o \
	src/archs/darwin/Darwin-main-help.o

CPPFLAGS += $(shell sdl-config --cflags)

LDFLAGS  += $(shell sdl-config --libs)

%.o:	%.m
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(TARGET):	$(TARGET_DIR)/Info.plist
$(TARGET):	$(TARGET_DIR)/PkgInfo
$(TARGET):	$(TARGET_DIR)/Resources/English.lproj/InfoPlist.strings
$(TARGET):	$(TARGET_DIR)/Resources/SDLMain.nib/classes.nib
$(TARGET):	$(TARGET_DIR)/Resources/SDLMain.nib/info.nib
$(TARGET):	$(TARGET_DIR)/Resources/SDLMain.nib/objects.nib
$(TARGET):	$(TARGET_DIR)/Resources/4p.icns

$(TARGET_DIR)/%:	archs/Darwin/%
	-@$(MKDIR_CMD)
	cp -p $< $@
