############################################################
CXX := c++
STRIP := strip
############################################################
MKDIR_CMD = test -d $(@D) || mkdir -p $(@D)
############################################################

CPPFLAGS += -I/boot/develop/tools/gnupro/include/SDL

LDFLAGS += \
    -L$(HOME)/config/lib -lSDL -lSDLmain
