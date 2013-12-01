# makefile for FormatDisk.ct (translated Texts : deutsch)
# $Date: 2$
# $Revision$
# $Id$

.SUFFIXES: .ct .catalog

.ct.catalog :
	@printf '\033[32mMake catalog: \033[31m\033[1mdeutsch\033[0m\n'
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

FormatDisk.catalog : FormatDisk.ct ../../../FormatDisk.cd

All: FormatDisk.catalog 
