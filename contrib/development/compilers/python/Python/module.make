LOCAL_FILES := \
	bltinmodule \
	ceval \
	codecs \
	compile \
	errors \
	exceptions \
	frozen \
	frozenmain \
	future \
	getargs \
	getcompiler \
	getcopyright \
	getmtime \
	getopt \
	getplatform \
	getversion \
	graminit \
	import \
	importdl \
	dynload_stub \
	marshal \
	modsupport \
	mystrtoul \
	mysnprintf \
	pyfpe \
	pystate \
	sigcheck \
	structmember \
	symtable \
	sysmodule \
	traceback \
	pythonrun

# Needs some work to compile.
#	thread \


# Already implemented in AROS (don't need these replacements).
#	strerror
#	strdup
#	strtod
#	getcwd
#	dup2
#	memmove
#	atof
#	fmod	
#	hyport

# Highly platform-specific stuff, not for AROS.
#	dynload_aix 
#	dynload_beos 
#	dynload_dl 
#	dynload_hpux 
#	dynload_mac 
#	dynload_next 
#	dynload_os2 
#	dynload_shlib 
#	dynload_win 


USER_CFLAGS += -IPython
FILES       += $(patsubst %, Python/%, $(LOCAL_FILES))
