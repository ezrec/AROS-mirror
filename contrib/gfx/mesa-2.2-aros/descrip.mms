# Makefile for Mesa for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl


all :
	if f$search("lib.dir") .eqs. "" then create/directory [.lib]
	set default [.src]
	mms
	set default [-.src-glu]
	mms
	set default [-.src-tk]
	mms
	set default [-.src-aux]
	mms
	set default [-.demos]
	mms
