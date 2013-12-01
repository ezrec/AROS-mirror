# makefile für Scalos (translated Texts : español)
# 01 Jan 2004 12:44:50

.SUFFIXES: .ct .catalog

.ct.catalog :
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

ScalosPattern.catalog : ScalosPattern.ct ../../../ScalosPattern.cd

All: ScalosPattern.catalog
