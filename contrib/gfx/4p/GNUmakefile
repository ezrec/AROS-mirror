# Makefile for 4P
#
#  2000 Nov 11  -  2001 Nov 11
#  jsl@umlautllama.com
#  Scott "Jerry" Lawrence
#

############################################################
prog:
all:
############################################################
NAME    := 4p
VERSION := 0.09

#HAS_NOWEB := 1
CC := arosgcc 
ARCH := $(shell uname -s)

ifdef HAS_NOWEB

NWS := \
	nws/null.nw \
	nws/end.nw

endif

DOC := doc/4p-dev.pdf

TARGET := Release/bin/$(ARCH)/4p
SRCS := \
        src/main.c			\
        src/testdlgs.c			\
        src/splash.c			\
        \
        src/jsui/utils.c		\
        src/jsui/gui/active.c		\
        src/jsui/gui/jsuibase.c		\
        src/jsui/gui/msg.c		\
        src/jsui/gui/uievents.c		\
        src/jsui/gui/placement.c	\
        src/jsui/gui/widgets/3dbox.c	\
        src/jsui/gui/widgets/buttons.c	\
        src/jsui/gui/widgets/editbox.c	\
        src/jsui/gui/widgets/frame.c	\
        src/jsui/gui/widgets/image.c	\
        src/jsui/gui/widgets/listbox.c	\
        src/jsui/gui/widgets/menu.c	\
        src/jsui/gui/widgets/palette.c	\
        src/jsui/gui/widgets/progress.c	\
        src/jsui/gui/widgets/slider.c	\
        src/jsui/gui/widgets/text.c	\
        src/jsui/gui/widgets/widgets.c	\
        src/jsui/gfx/floodfil.c		\
        src/jsui/gfx/font.c		\
        src/jsui/gfx/gfx_base.c		\
        src/jsui/gfx/gfx_prim.c		\
        src/jsui/gfx/page.c		\
        src/jsui/gfx/palette.c		\
        src/jsui/gfx/pen.c		\
        src/jsui/gfx/timing.c		\
        \
        src/dialogs/acceptor.c		\
        src/dialogs/fontreq.c		\
        src/dialogs/filereq.c		\
        src/dialogs/menus.c		\
        src/dialogs/palette.c		\
        src/dialogs/resolutn.c		\
        src/dialogs/toolbar.c		\
        src/dialogs/toolbar_gfx.c	\
        src/dialogs/brushes.c		\
        src/dialogs/brushes_gfx.c	\
        \
	src/paint/menus.c		\
        src/paint/brushes.c		\
        src/paint/cursors.c		\
        src/paint/events.c		\
        src/paint/fileio.c		\
        src/paint/paint.c		\
        src/paint/tool_cb.c		\
        src/paint/tools.c		\
        src/paint/zoom.c		\
        \
        src/paint/iff/blockio.c		\
        src/paint/iff/endian.c		\
        src/paint/iff/iff_load.c	\
        src/paint/iff/iff_save.c	\
        src/paint/iff/iff_type.c	\
        \
        src/paint/tools/airbrush.c	\
        src/paint/tools/brush.c		\
        src/paint/tools/brushgeo.c	\
        src/paint/tools/circle.c	\
        src/paint/tools/color.c		\
        src/paint/tools/draw.c		\
        src/paint/tools/drawmode.c	\
        src/paint/tools/fill.c		\
        src/paint/tools/rectangl.c	\
        src/paint/tools/screen.c	\
        src/paint/tools/text.c		\
        src/paint/tools/ui.c		\
        src/paint/tools/vector.c

OBJS := $(SRCS:%.c=%.o)

HDRS := 

ALLDATA := \
	Release/bin/$(ARCH)/Images/Animals			\
	Release/bin/$(ARCH)/Images/Backdrop			\
	Release/bin/$(ARCH)/Images/DpaintTitle			\
	Release/bin/$(ARCH)/Images/KingTut			\
	Release/bin/$(ARCH)/Images/StarFlight			\
	Release/bin/$(ARCH)/Images/SundayAfternoon.Seurat	\
	Release/bin/$(ARCH)/Images/Venus			\
	Release/bin/$(ARCH)/Images/Xmas_Background		\
	Release/bin/$(ARCH)/Images/Xmas_Set			\
	Release/bin/$(ARCH)/Images/dpaint


#CXXFLAGS := -O -DNDEBUG=1
CXXFLAGS := 
CPPFLAGS := 

# Since my personal belief is that if your build has any warnings
# at all, then you've done something wrong, I have used the two
#
# Due to the fact that the people working on libSDL don't agree
# with this, I had to also use the -Wno-long-long, since they use
# the type 'long long'.


CFLAGS  := -g \
	-funroll-loops -fomit-frame-pointer -ffast-math \
	-Isrc -Isrc/dialogs \
	-Isrc/jsui -Isrc/jsui/gui -Isrc/jsui/gfx \
	-Isrc/paint \
	-Isrc/paint/iff -Isrc/paint/tools \
	`sdl-config --cflags`   

############################################################
include archs/$(ARCH)/variables.GNU
############################################################

all: prog $(DOC)

prog:	$(TARGET) $(DATA)

size:	all
	tar cf - README Release | wc -c

clean:	tidy
	rm -rf Release/bin Release/data
ifdef HAS_NOWEB
	rm -rf code doc
endif

tidy:
	rm -rf $(OBJS) \
		$(DOC:%.pdf=%.tex) \
		$(DOC:%.pdf=%.aux) \
		$(DOC:%.pdf=%.log) \
		$(DOC:%.pdf=%.toc)

############################################################

$(TARGET):	$(OBJS)
	-@$(MKDIR_CMD)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

#	$(STRIP) $@

ifdef HAS_NOWEB

$(HDRS) $(SRCS):	$(NWS)
	-@$(MKDIR_CMD)
	notangle -L -R$(@F) $^ | cpif $@

$(DOC):	$(DOC:%.pdf=%.tex)
	-@$(MKDIR_CMD)
	( \
	    cd $(@D); \
	    pdflatex $(<F) ; \
	    pdflatex $(<F) ; \
	)

$(DOC:%.pdf=%.tex):	$(NWS)
	-@$(MKDIR_CMD)
	noweave -index $^ | cpif $@

endif

############################################################

%.o:	%.cpp
	-@$(MKDIR_CMD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

Release/bin/$(ARCH)/Images/%:	Images/%
	-@$(MKDIR_CMD)
	cp -p $< $@

############################################################
test:	prog
	$(TARGET)

############################################################


FLOPPY   := /mnt/floppy
BAK_NAME := $(NAME)_$(VERSION)

backup: clean
	touch ../$(BAK_NAME).tar.gz
	mv ../$(BAK_NAME).tar.gz ../$(BAK_NAME)_old.tar.gz
	cd .. ; tar -cvf $(BAK_NAME).tar $(NAME)/* ; gzip $(BAK_NAME).tar

floppy: backup
	mount $(FLOPPY)
	cp ../$(BAK_NAME).tar.gz $(FLOPPY)
	sync
	umount $(FLOPPY)

.PHONY: backup
.PHONY: floppy
############################################################

.PHONY: all
.PHONY: prog
.PHONY: size
.PHONY: clean
.PHONY: tidy

.SECONDARY: $(OBJS)
.SECONDARY: $(SRCS)
.SECONDARY: $(HDRS)
.SECONDARY: $(DOC:%.pdf=%.tex)
.SECONDARY: $(DOC:%.pdf=%.aux)
.SECONDARY: $(DOC:%.pdf=%.log)
.SECONDARY: $(DOC:%.pdf=%.toc)

############################################################
