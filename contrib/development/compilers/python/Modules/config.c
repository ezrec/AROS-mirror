/* -*- C -*- ***********************************************
Copyright (c) 2000, BeOpen.com.
Copyright (c) 1995-2000, Corporation for National Research Initiatives.
Copyright (c) 1990-1995, Stichting Mathematisch Centrum.
All rights reserved.

See the file "Misc/COPYRIGHT" for information on usage and
redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.
******************************************************************/

/* Handwritten module configuration for AROS */
/* NOT GENERATED AUTOMATICALLY !!! DO NOT DELETE !!! */

#include "Python.h"

extern void initenvironment(void);
extern void init_codecs(void);
extern void init_sre(void);
extern void init_testcapi(void);
extern void init_weakref(void);
extern void initarray(void);
extern void initaudioop(void);
extern void initbinascii(void);
extern void initcStringIO(void);
extern void initcPickle(void);
extern void initerrno(void);
extern void initgc(void);
extern void initimageop(void);
extern void initmd5(void);
extern void initnew(void);
extern void initoperator(void);
extern void initparser(void);
extern void initpure(void);
extern void initregex(void);
extern void initrgbimg(void);
extern void initrotor(void);
//extern void initselect(void);
extern void initsha(void);
extern void initstrop(void);
extern void initstruct(void);
extern void init_symtable(void);
//extern void inittime(void);
extern void inittiming(void);
extern void initunicodedata(void);
extern void initxreadlines(void);
extern void initzlib(void);
extern void PyMarshal_Init(void);
extern void initimp(void);

struct _inittab _PyImport_Inittab[] = {
	{"environment", initenvironment},
	{"_codecs",     init_codecs},
	{"_sre",        init_sre},
	{"_testcapi",   init_testcapi},
	{"_weakref",    init_weakref},
	{"array",       initarray},
	{"audioop",     initaudioop},
	{"binascii",    initbinascii},
	{"cPickle",     initcPickle},
	{"cStringIO",   initcStringIO},
	{"errno",       initerrno},
	{"gc",          initgc},
	{"imageop",     initimageop},
	{"md5",         initmd5},
	{"new",         initnew},
	{"operator",    initoperator},
	{"parser",      initparser},
	{"pure",        initpure},
	{"regex",       initregex},
	{"rgbimg",      initrgbimg},
	{"rotor",       initrotor},
//	{"select",      initselect},
	{"sha",         initsha},
	{"strop",       initstrop},
	{"struct",      initstruct},
	{"symtable",    init_symtable},
//	{"time",        inittime},
	{"timing",      inittiming},
	{"unicodedata", initunicodedata},
	{"xreadlines",  initxreadlines},
	{"zlib",        initzlib},

	/* This module lives in marshal.c */
	{"marshal", PyMarshal_Init},

	/* This lives in import.c */
	{"imp", initimp},

	/* These entries are here for sys.builtin_module_names */
	{"__main__", NULL},
	{"__builtin__", NULL},
	{"sys", NULL},
	{"exceptions", init_exceptions},

	/* Sentinel */
	{0, 0}
};
