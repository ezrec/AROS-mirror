
# **************************************************************************
#
# Mailtext - An MUI Custom Class for eMessage display
# Copyright (C) 1996-2001 by Olaf Peters
# Copyright (C) 2002-2006 by Mailtext Open Source Team
#
# GCC & OS4 Portage by Alexandre Balaban
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
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Mailtext OpenSource project :  http://sourceforge.net/projects/mailtext/
#
# $Id$
#
# GCC Makefile for MCC_Mailtext
#
# *************************************************************************


all:
	@echo ""
	@echo "You should choose one of the supported targets : 68k or os4"
	@echo "Example : make os4"
	@echo ""
	@echo "Alternatively you can directly tell make to use the specific"
	@echo "makefile using the '-f' flag"
	@echo "Example : make -f makefile.68k"
	@echo ""

68k:
	@echo "Compiling for 68k"
	@$(MAKE) -f Makefile.68k

68k-strip:
	@echo "Compiling for 68k with stripped binaries"
	@$(MAKE) -f Makefile.68k final -s

os4:
	@echo "Compiling for 0S4"
	@$(MAKE) -f Makefile.os4 -s

os4-strip:
	@echo "Compiling for 0S4 with stripped binaries"
	@$(MAKE) -f Makefile.os4 final -s

docs:
	@$(MAKE) -f Makefile.os4 docs -s

guides:
	@$(MAKE) -f Makefile.os4 guides -s

catalogs:
	@$(MAKE) -f Makefile.os4 catalogs -s

release-all:
	@$(MAKE) -f Makefile.os4 release -s
	@$(MAKE) -f Makefile.68k getbin -s

release-os4:
	@$(MAKE) -f Makefile.os4 release -s

release-68k:
	@$(MAKE) -f Makefile.68k release -s

archive:
	@$(MAKE) -f Makefile.68k getbin -s
	@$(MAKE) -f Makefile.os4 archive -s

install-68k:
	@$(MAKE) -f Makefile.68k install -s

install-os4:
	@$(MAKE) -f Makefile.os4 install -s

clean:
	@$(MAKE) -f Makefile.68k clean -s
	@$(MAKE) -f Makefile.os4 clean -s

clean-os4:
	@$(MAKE) -f Makefile.os4 clean -s

clean-68k:
	@$(MAKE) -f Makefile.68k clean -s

cleanall:
	@$(MAKE) -f Makefile.68k clean -s
	@$(MAKE) -f Makefile.os4 cleanall -s

