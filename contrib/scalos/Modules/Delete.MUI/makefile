# makefile for Scalos delete.MUI module
# $Date$

#############################################################

SUBDIRS =	Source \

#############################################################

.PHONY: All install clean

#############################################################

All:
	@for name in $(SUBDIRS) ; do \
		$(MAKE) -s --directory=$$name; \
	done

#############################################################

install:
	@for name in $(SUBDIRS) ; do \
		$(MAKE) -s install --directory=$$name; \
	done

#############################################################

clean:
	@for name in $(SUBDIRS) ; do \
		$(MAKE) -s clean --directory=$$name; \
	done

#############################################################
