#/***************************************************************************
#
# TheBar.mcc - Next Generation Toolbar MUI Custom Class
# Copyright (C) 2003-2005 Alfonso Ranieri
# Copyright (C) 2005-2009 by TheBar.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# TheBar class Support Site:  http://www.sf.net/projects/thebar
#
# $Id$
#
#***************************************************************************/

.PHONY: all
all: button mcc mcp demo

.PHONY: button
button:
	@$(MAKE) -C mcc/button --no-print-directory

.PHONY: mcc
mcc:
	@$(MAKE) -C mcc --no-print-directory

.PHONY: mcp
mcp:
	@$(MAKE) -C mcp --no-print-directory

.PHONY: demo
demo:
	@$(MAKE) -C demo --no-print-directory

.PHONY: clean
clean:
	@$(MAKE) -C mcc/button --no-print-directory clean
	@$(MAKE) -C mcc --no-print-directory clean
	@$(MAKE) -C mcp --no-print-directory clean
	@$(MAKE) -C demo --no-print-directory clean

.PHONY: cleanall
cleanall:
	@$(MAKE) -C mcc/button --no-print-directory cleanall
	@$(MAKE) -C mcc --no-print-directory cleanall
	@$(MAKE) -C mcp --no-print-directory cleanall
	@$(MAKE) -C demo --no-print-directory cleanall

.PHONY: install
install:
	@$(MAKE) -C mcc/button --no-print-directory install
	@$(MAKE) -C mcc --no-print-directory install
	@$(MAKE) -C mcp --no-print-directory install
	@$(MAKE) -C demo --no-print-directory install

.PHONY: bumprev
bumprev:
	@sh tools/bumprev.sh all

.PHONY: release
release:
	@sh tools/mkrelease.sh
