"""
AMIGA LIBRARY INTERFACE FOR dos.library
©Irmen de Jong, disclaimer applies!

$VER: Dos.py 1.3 (1.10.00)
"""


import string
import os,time
import errno

from Doslib import *

# Modes for SetConsoleMode()
MODE_CON = 0
MODE_RAW = 1

# Bits that signal you that a user has issued a break
SIGBREAKF_CTRL_C = 1<<12
SIGBREAKF_CTRL_D = 1<<13
SIGBREAKF_CTRL_E = 1<<14
SIGBREAKF_CTRL_F = 1<<15
	
# FILE PROTECTION BITS
# USUAL
FIBF_SCRIPT	= 1<<6
FIBF_PURE	= 1<<5
FIBF_ARCHIVE	= 1<<4
FIBF_READ	= 1<<3
FIBF_WRITE	= 1<<2
FIBF_EXECUTE	= 1<<1
FIBF_DELETE	= 1<<0
# "OTHER"
FIBF_OTR_READ	= 1<<15
FIBF_OTR_WRITE	= 1<<14
FIBF_OTR_EXECUTE= 1<<13
FIBF_OTR_DELETE	= 1<<12
# "GROUP"
FIBF_GRP_READ	= 1<<11
FIBF_GRP_WRITE	= 1<<10
FIBF_GRP_EXECUTE= 1<<9
FIBF_GRP_DELETE	= 1<<8

# FLAGS FOR DateToStr AND StrToDate
DTF_SUBST	= 1<<0	# substitute Today, Tomorrow, etc.
DTB_FUTURE	= 1<<1	# day of the week is in future

# FORMATS FOR DateToStr AND StrToDate
FORMAT_DOS	= 0	# dd-mmm-yy
FORMAT_INT	= 1	# yy-mm-dd
FORMAT_USA	= 2	# mm-dd-yy
FORMAT_CDN	= 3	# dd-mm-yy


### EXAMINE struct members (FileInfoBlock)
fib_FileName	= 0
fib_Size		= 1
fib_DirEntryType= 2		# if <0: file, if >0: dir
fib_Protection	= 3
fib_DiskKey		= 4
fib_NumBlocks	= 5
fib_Date		= 6
fib_Comment		= 7
fib_OwnerUID	= 8
fib_OwnerGID	= 9


### INFO struct members (InfoData)
id_NumSoftErrors	= 0
id_UnitNumber		= 1
id_DiskState		= 2		# see defines below
id_NumBlocks		= 3
id_NumBlocksUsed	= 4
id_BytesPerBlock	= 5
id_DiskType			= 6		# 4-byte FileSystem type code
#id_VolumeNode  not used in Python
id_InUse			= 7		# flag, zero if not in use

### INFO struct DiskState types
ID_WRITE_PROTECTED	= 80
ID_VALIDATING		= 81
ID_VALIDATED		= 82


#### ARGPARSER ##################################

class ArgParser:
	"""
	Argument string parser class.
	To be used for parsing dos.library/ReadArgs() arguments
	(the Amiga style for command lines).
	"""
	def __init__(self,template):            # create
		self.new(template)
	def new(self,template):                 # new template
		self.template=template
		self.reset()
	def reset(self):                        # reset types & defaults
		self.defaults,self.types = self.parsetempl(self.template)
	def parse(self,args):
		result=ReadArgs(self.template,args,self.types)
		for k in result.keys():
			if not result[k]:
				dflt = self.defaults[k]
				if type(dflt)!=type([]):
					result[k]=dflt
				else:
					result[k]=dflt[:]   # make slice copy of list
		return result               

	# Below this point are private members.

	def parsetempl(self,templ):
		# Parse template.
		# This function builds the default argument dictionary and
		# the argument type list.

		# first check special case: empty template
		if not templ:
			return ({},())

		for c in templ:
			if not c in '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_.,=/ ':
				raise ValueError,'invalid char in template ('+c+')'

		# Split the template twice (first by ',' then by '/')
		templ=map(lambda x: string.split(x,'/'),string.split(templ,','))
		# Build the type list
		types=[]; defdict={}; lists=0; keywdic={}
		defaults = {'S':0, 'T':0, 'N':None, 'X':None, 'A':[], 'I': []}
		for a in templ:
			if a[0]=='':
				raise ValueError,'missing keyword in template'

			# Now convert argument spec --> type & modifiers.
			# 6 different output types are possible:
			# X     - string            \
			# S,T   - bool (integer)     > can have /F as modifier
			#	(note: currently \T is NOT SUPPORTED)
			# N     - integer           /
			# X/M   - array of strings ('A')
			# N/M   - array of ints ('I')
			# All 6 can have /A or /K as additional modifiers.
			# However, /F /A and /K have no influence on the returned type. (/M does)

			type=None; modifiers='';
			for s in map(string.upper,a[1:]):
				if not s:
					raise ValueError,'invalid template'
				elif s=='T':
					raise SystemError,'/T not (yet) supported, sorry'
				elif s in 'SNT':        # possible template option types
					if type:
						raise ValueError,'invalid switch combination at '+a[0]
					type=s
				elif s in 'KAMF':       # template option modifiers
					if s in modifiers or  \
					   (s=='M' and 'F' in modifiers) or \
					   (s=='F' and 'M' in modifiers):
						# do not allow /F/M or /M/F or /A/A etc 
						raise ValueError,'invalid switch combination at '+a[0]
					modifiers=modifiers+s
				else:
					raise ValueError,'unknown switch /'+s

			if not type:
				type = 'X'      # special type: string (default type)

			if 'M' in modifiers:
				lists=lists+1
				if lists>1:
					raise ValueError,'multiple /M switches'
				if type=='X': type='A'      # array of strings
				elif type=='N': type='I'    # array of ints
				else:
					raise ValueError,'wrong /M combination'

			# All done, add type to type list, and find default value.

			types.append(a[0],type)
			try:
				keywdic[a[0]]=keywdic[a[0]]+1
			except KeyError:
				keywdic[a[0]]=1
			if not 'A' in modifiers:
				defdict[a[0]]=defaults[type]
			
		keywdic=keywdic.values()
		if keywdic.count(1) != len(keywdic):
			raise ValueError,'clashing keywords'

		return (defdict,tuple(types))




##################### Various Functions #################

def touch(file, tme=None):
	if not tme:
		tme=time.time()
	tme = time2DS(tme)
	# first, check if the file exists
	try:
		os.stat(file)
		# file exists... continue
	except os.error, x:
		if(x[0]==errno.ENOENT):
			# file does not exist, create it first
			open(file,'w')
		else:
			# other error...
			raise os.error,x

	# now, set the filedate
	SetFileDate(file,tme)


def AddBuffers(drive, buffers):
	return os.system('c:addbuffers >NIL: "%s" %d'%(drive,buffers))

def AssignAdd(name, target):
	return os.system('c:assign >NIL: "%s" "%s" ADD'%(name,target))

def AssignRemove(name):
	return os.system('c:assign >NIL: "%s" REMOVE'%name)

