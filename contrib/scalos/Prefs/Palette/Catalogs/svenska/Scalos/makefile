# makefile for Scalos (translated Texts : svenska)
# $Date$

.SUFFIXES: .ct .catalog

.ct.catalog :
	@printf '\033[32mMake catalog: \033[31m\033[1msvenska\033[0m\n'
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

ScalosPalette.catalog : ScalosPalette.ct ../../../ScalosPalette.cd

All: ScalosPalette.catalog
