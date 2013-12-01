# makefile for Scalos (translated Texts : français)
# $Date$

.SUFFIXES: .ct .catalog

.ct.catalog :
	@printf '\033[32mMake catalog: \033[31m\033[1mfrançais\033[0m\n'
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

ScalosPopupMenu.catalog : ScalosPopupMenu.ct ../../../ScalosPopupMenu.cd

All: ScalosPopupMenu.catalog 
