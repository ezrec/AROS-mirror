############################################################
CXX := ../../../bin/linux-i386/tools/i386-linux-aros-gcc
STRIP := strip
LIBS := `sdl-config --libs`
############################################################
MKDIR_CMD = test -d $(@D) || mkdir -p $(@D)
############################################################
CFLAGS += -Wall -Wno-long-long -DSKIP_REQUEST_RESOLUTION_SPLASH   

