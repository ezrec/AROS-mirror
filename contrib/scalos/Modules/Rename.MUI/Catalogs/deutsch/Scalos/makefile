# makefile for Rename.module (translated Texts : deutsch)
# $Date$
# $Revision$

.SUFFIXES: .ct .catalog

.ct.catalog :
	@printf '\033[32mMake catalog: \033[31m\033[1mdeutsch\033[0m\n'
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

Rename.catalog : Rename.ct ../../../Rename.cd

All: Rename.catalog
