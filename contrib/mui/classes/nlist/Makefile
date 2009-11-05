#/***************************************************************************
#
# NList MUI custom classes
#
# Copyright (C) 1996-2001 by Gilles Masson (NList.mcc)
# Copyright (C) 1999-2001 by Carsten Scholling (NListtree.mcc)
# Copyright (C) 2006      by Daniel Allsopp (NBitmap.mcc)
# Copyright (C) 2001-2008 by NList Open Source Team
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
# NList classes Support Site:  http://www.sf.net/projects/nlist-classes
#
# $Id$
#
#***************************************************************************/

CP = cp -a

SUBDIRS = nbalance_mcc \
          nbitmap_mcc \
          nfloattext_mcc \
          nlist_mcc \
          nlisttree_mcc \
          nlisttree_mcp \
          nlistview_mcc \
          nlistviews_mcp \
          demo

.PHONY: all
all: $(SUBDIRS) catalogs

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@$(MAKE) -C $@

.PHONY: catalogs
catalogs:
	@$(MAKE) -C nlisttree_mcp catalogs
	@$(MAKE) -C nlistviews_mcp catalogs

.PHONY: release catalogs demo
release: $(SUBDIRS)
	$(CP) nlistview_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nlist_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nlisttree_mcp/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nlistviews_mcp/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nlisttree_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nfloattext_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nbitmap_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nbalance_mcc/bin_os3/*.mc? release/MCC_NList/Libs/MUI/AmigaOS3/
	$(CP) nlistview_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nlist_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nlisttree_mcp/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nlistviews_mcp/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nlisttree_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nfloattext_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nbitmap_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nbalance_mcc/bin_mos/*.mc? release/MCC_NList/Libs/MUI/MorphOS/
	$(CP) nlistview_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nlist_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nlisttree_mcp/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nlistviews_mcp/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nlisttree_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nfloattext_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nbitmap_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nbalance_mcc/bin_os4/*.mc? release/MCC_NList/Libs/MUI/AmigaOS4/
	$(CP) nlistview_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nlist_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nlisttree_mcp/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nlistviews_mcp/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nlisttree_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nfloattext_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nbitmap_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nbalance_mcc/bin_aros-i386/*.mc? release/MCC_NList/Libs/MUI/AROS-i386/
	$(CP) nlistview_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nlist_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nlisttree_mcp/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nlistviews_mcp/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nlisttree_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nfloattext_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nbitmap_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nbalance_mcc/bin_aros-ppc/*.mc? release/MCC_NList/Libs/MUI/AROS-ppc/
	$(CP) nlistview_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nlist_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nlisttree_mcp/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nlistviews_mcp/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nlisttree_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nfloattext_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nbitmap_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) nbalance_mcc/bin_aros-x86_64/*.mc? release/MCC_NList/Libs/MUI/AROS-x86_64/
	$(CP) demo/bin_os4/*-Demo release/MCC_NList/Demo/AmigaOS4/
	$(CP) demo/bin_os3/*-Demo release/MCC_NList/Demo/AmigaOS3/
	$(CP) demo/bin_mos/*-Demo release/MCC_NList/Demo/MorphOS/
	$(CP) demo/bin_aros-i386/*-Demo release/MCC_NList/Demo/AROS-i386/
	$(CP) demo/bin_aros-ppc/*-Demo release/MCC_NList/Demo/AROS-ppc/
	$(CP) demo/bin_aros-x86_64/*-Demo release/MCC_NList/Demo/AROS-x86_64/
	$(CP) demo/*.c release/MCC_NList/Developer/C/Examples/
	$(CP) demo/*.h release/MCC_NList/Developer/C/Examples/
	$(CP) demo/Makefile release/MCC_NList/Developer/C/Examples/
	$(CP) nlisttree_mcp/locale/*.cd release/MCC_NList/Catalogs/
	$(CP) nlistviews_mcp/locale/*.cd release/MCC_NList/Catalogs/
	$(CP) docs/MCC_*.doc release/MCC_NList/Developer/Autodocs/
	$(CP) include/mui/N*.h release/MCC_NList/Developer/C/Include/MUI/
	$(CP) docs/ReadMe release/MCC_NList/
	$(CP) ChangeLog release/MCC_NList/
	$(CP) AUTHORS release/MCC_NList/


.PHONY: clean
clean:
	@$(MAKE) -C nbalance_mcc clean
	@$(MAKE) -C nbitmap_mcc clean
	@$(MAKE) -C nfloattext_mcc clean
	@$(MAKE) -C nlist_mcc clean
	@$(MAKE) -C nlisttree_mcc clean
	@$(MAKE) -C nlisttree_mcp clean
	@$(MAKE) -C nlistview_mcc clean
	@$(MAKE) -C nlistviews_mcp clean
	@$(MAKE) -C demo clean

.PHONY: distclean
distclean:
	@$(MAKE) -C nbalance_mcc distclean
	@$(MAKE) -C nbitmap_mcc distclean
	@$(MAKE) -C nfloattext_mcc distclean
	@$(MAKE) -C nlist_mcc distclean
	@$(MAKE) -C nlisttree_mcc distclean
	@$(MAKE) -C nlisttree_mcp distclean
	@$(MAKE) -C nlistview_mcc distclean
	@$(MAKE) -C nlistviews_mcp distclean
	@$(MAKE) -C demo distclean

