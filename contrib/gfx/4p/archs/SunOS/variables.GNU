############################################################
CC  := gcc
CXX := CC
STRIP := strip
LIBS := `sdl-config --libs`
############################################################
MKDIR_CMD = test -d $(@D) || mkdir -p $(@D)
############################################################
CFLAGS += -Wall -pedantic-errors -Wno-long-long    

