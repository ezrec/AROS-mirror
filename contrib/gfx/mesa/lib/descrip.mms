# Makefile for Mesa for VMS
# contributed by Jouk Jansen  joukj@crys.chem.uva.nl


all :
	if f$search("lib.dir") .eqs. "" then create/directory [.lib]
	set default [.src]
	mms
	set default [-.src-glu]
	mms
# PIPE is avalailable on VMS7.0 and higher. For lower versions split the
#command in two conditional command.   JJ
	if f$search("[-]SRC-GLUT.DIR") .nes. "" then pipe set default [-.src-glut] ; mms
	if f$search("[-]DEMOS.DIR") .nes. "" then pipe set default [-.demos] ; mms

