#
# GCC makefile
#

SHELL = /bin/sh

#

all:
	$(MAKE) -C NList_mcc -f makefile
	$(MAKE) -C NListview_mcc -f makefile
	$(MAKE) -C NFloattext_mcc -f makefile
	$(MAKE) -C NListviews_mcp -f makefile
	$(MAKE) -C Floattext_mui -f makefile

install:
	$(MAKE) -C NList_mcc -f makefile install
	$(MAKE) -C NListview_mcc -f makefile install
	$(MAKE) -C NFloattext_mcc -f makefile install
	$(MAKE) -C NListviews_mcp -f makefile install
	$(MAKE) -C Floattext_mui -f makefile install

uninstall:
	$(MAKE) -C NList_mcc -f makefile uninstall
	$(MAKE) -C NListview_mcc -f makefile uninstall
	$(MAKE) -C NFloattext_mcc -f makefile uninstall
	$(MAKE) -C NListviews_mcp -f makefile uninstall
	$(MAKE) -C Floattext_mui -f makefile uninstall

clean:
	$(MAKE) -C NList_mcc -f makefile clean
	$(MAKE) -C NListview_mcc -f makefile clean
	$(MAKE) -C NFloattext_mcc -f makefile clean
	$(MAKE) -C NListviews_mcp -f makefile clean
	$(MAKE) -C Floattext_mui -f makefile clean

