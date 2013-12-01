# makefile for Exchange.module (translated Texts : français)
# $Date: 17 Aug 2004 20:12:47

.SUFFIXES: .ct .catalog

.ct.catalog :
	@printf '\033[32mMake catalog: \033[31m\033[1mfrançais\033[0m\n'
	CatComp ///$*.cd $*.ct CATALOG $*.catalog VB=2
	avail flush

Exchange.catalog : Exchange.ct ../../../Exchange.cd

All: Exchange.catalog
