LOCAL_FILES := \
	__future__.py \
	Bastion.py \
	ConfigParser.py \
	Cookie.py \
	Dos.py \
	MimeWriter.py \
	Queue.py \
	StringIO.py \
	UserDict.py \
	UserList.py \
	UserString.py \
	aifc.py \
	amigapath.py \
	atexit.py \
	base64.py \
	bdb.py \
	binhex.py \
	bisect.py \
	calendar.py \
	cgi.py \
	chunk.py \
	cmd.py \
	code.py \
	codecs.py \
	codeop.py \
	colorsys.py \
	commands.py \
	compileall.py \
	copy.py \
	copy_reg.py \
	difflib.py \
	dircache.py \
	dis.py \
	doctest.py \
	anydbm.py \
	dumbdbm.py \
	filecmp.py \
	fileinput.py \
	fnmatch.py \
	formatter.py \
	fpformat.py \
	getopt.py \
	glob.py \
	gzip.py \
	htmlentitydefs.py \
	htmllib.py \
	ihooks.py \
	imghdr.py \
	imputil.py \
	inspect.py \
	keyword.py \
	knee.py \
	linecache.py \
	mailbox.py \
	mailcap.py \
	mhlib.py \
	mimetools.py \
	mimetypes.py \
	mimify.py \
	multifile.py \
	mutex.py \
	netrc.py \
	os.py \
	pdb.py \
	pickle.py \
	pipes.py \
	popen2.py \
	pprint.py \
	pre.py \
	profile.py \
	pstats.py \
	py_compile.py \
	pyclbr.py \
	pydoc.py \
	quopri.py \
	random.py \
	re.py \
	reconvert.py \
	regex_syntax.py \
	regsub.py \
	repr.py \
	rexec.py \
	rfc822.py \
	robotparser.py \
	sched.py \
	sgmllib.py \
	shelve.py \
	shlex.py \
	shutil.py \
	site.py \
	sndhdr.py \
	sre.py \
	sre_compile.py \
	sre_constants.py \
	sre_parse.py \
	stat.py \
	statcache.py \
	string.py \
	sunau.py \
	sunaudio.py \
	symbol.py \
	symtable.py \
	tabnanny.py \
	tempfile.py \
	toaiff.py \
	token.py \
	tokenize.py \
	traceback.py \
	types.py \
	tzparse.py \
	unittest.py \
	urlparse.py \
	user.py \
	uu.py \
	warnings.py \
	wave.py \
	weakref.py \
	webbrowser.py \
	whichdb.py \
	whrandom.py \
	xdrlib.py \
	xmllib.py \
	zipfile.py

# Network related modules: 
#	SimpleHTTPServer.py \
#	BaseHTTPServer.py \
#	CGIHTTPServer.py \
#	SocketServer.py \
#	asynchat.py \
#	asyncore.py \
#	httplib.py \
#	gopherlib.py \
#	ftplib.py \
#	imaplib.py \
#	nntplib.py \
#	poplib.py \
#	smtpd.py \
#	smtplib.py \
#	socket.py \
#	telnetlib.py \
#	urllib.py \
#	urllib2.py \
			
# Possibly portable modules:
#	dbhash.py \
#	gettext.py \
#	getpass.py \
#	locale.py \
#	rlcompleter.py \
#	threading.py \

# Obsolete modules:
#	stringold.py \


# Highly platform specific modules:
#	TERMIOS.py \
#	audiodev.py \
#	dospath.py \
#	macpath.py \
#	macurl2path.py \
#	ntpath.py \
#	nturl2path.py \
#	posixfile.py \
#	posixpath.py \
#	pty.py \
#	statvfs.py \
#	tty.py \
				
MODULE_FILES += $(LOCAL_FILES)

include Lib/xml/module.make
include Lib/encodings/module.make
include Lib/test/module.make
