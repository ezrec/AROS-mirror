TOP=../..
CURDIR=apps/Games

SUBDIRS = Jump Mine Quad Wumpus

all:	setup
	@$(MAKE) $(MFLAGS) subdirs TARGET=all
	
setup :
	@$(MAKE) $(MFLAGS) subdirs TARGET=setup

clean:
	@$(MAKE) $(MFLAGS) subdirs TARGET=clean

subdirs:
	@for dir in $(SUBDIRS) ; do \
	    echo "Making $(TARGET) in $$dir..." ; \
	    if ( cd $$dir && \
		$(MAKE) $(MFLAGS) \
		TARGET=$(TARGET) $(TARGET) ) ; \
	    then true ; else exit 1 ; fi ; \
	done
