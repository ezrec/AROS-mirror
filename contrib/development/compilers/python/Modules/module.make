LOCAL_FILES := \
	main \
	python \
	config \
	environment \
	amigamodule \
	Doslibmodule \
	IntuitionLibrary \
	_codecsmodule \
	_sre \
	_testcapimodule \
	_weakref \
	arraymodule \
	audioop \
	binascii \
	cPickle \
	cStringIO \
	cgensupport \
	errnomodule \
	gcmodule \
	getbuildinfo \
	getpath \
	imageop \
	md5c \
	md5module \
	newmodule \
	operator \
	parsermodule \
	puremodule \
	regexmodule \
	regexpr \
	rgbimgmodule \
	rotormodule \
	shamodule \
	stropmodule \
	structmodule \
	symtablemodule \
	timemodule \
	timingmodule \
	unicodedata \
	xreadlinesmodule \
	yuvconvert \
	zlibmodule \
	mathmodule \
	cmathmodule \
	pypcre \
	pcremodule \
	pyexpat
	
# Modules that need more work to compile.
#	selectmodule \
#	fcntlmodule

# Modules that need a bit of work to be ported.
#	socketmodule
#	threadmodule
#	cryptmodule

# Modules that might be portable to AROS (with a lot of work).
#	bsddbmodule 
#	dbmmodule 
#	gdbmmodule
#	_cursesmodule
#	_curses_panel
#	_localemodule
#	fpectlmodule
#	fpetestmodule
#	mmapmodule
#	mpzmodule
#	nismodule
#	resource
#	syslogmodule

# Highly platform-specific modules, probably not portable to AROS.
#	tclNotify 
#	termios 
#	sunaudiodev 
#	linuxaudiodev 
#	posixmodule 
#	cdmodule 
#	_tkinter 
#	tkappinit
#	almodule
#	flmodule
#	sgimodule
#	clmodule
#	fmmodule
#	glmodule
#	imgfile
#	readline
#	signalmodule
#	svmodule
#	dlmodule
#	grpmodule
#	pwdmodule

USER_CFLAGS += -IModules
FILES       += $(patsubst %, Modules/%, $(LOCAL_FILES))
