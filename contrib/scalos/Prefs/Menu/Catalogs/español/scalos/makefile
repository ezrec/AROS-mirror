# makefile für Scalos (translated Texts : español)
# 01 Jan 2004 12:40:23

.SUFFIXES: .ct .catalog

.ct.catalog :
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

ScalosMenu.catalog : ScalosMenu.ct ../../../ScalosMenu.cd

All: ScalosMenu.catalog 
