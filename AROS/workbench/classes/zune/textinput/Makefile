#/***************************************************************************
#
# TextInput.mcc - TextInput MUI Custom Class
# Copyright (C) 1997-2001 Oliver Wagner
# Copyright (C) 2009-2010 by TextInput.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# TextInput class Support Site:  http://www.sf.net/projects/textinput-mcc
# 
# $Id$
#
#***************************************************************************/

.PHONY: all
all: mcc mcp demo

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
	@$(MAKE) -C mcc --no-print-directory clean
	@$(MAKE) -C mcp --no-print-directory clean
	@$(MAKE) -C demo --no-print-directory clean

.PHONY: cleanall
cleanall:
	@$(MAKE) -C mcc --no-print-directory cleanall
	@$(MAKE) -C mcp --no-print-directory cleanall
	@$(MAKE) -C demo --no-print-directory cleanall

.PHONY: install
install:
	@$(MAKE) -C mcc --no-print-directory install
	@$(MAKE) -C mcp --no-print-directory install
	@$(MAKE) -C demo --no-print-directory install
