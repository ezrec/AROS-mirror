'''Read in all archive files and takes them apart.

To use, import this module and run genadocs.ReadAutoDocs(). This will
return an object with these properties:

    libs - A list of all libraries found while reading the archives.
	The list items are of the type Library.

'''

import os.path, string

def findArchives (*dirs):
    def visit (list, path, entries):
	for entry in entries:
	    if entry[-5:] == '.arch':
		list.append (os.path.join (path, entry))

    list = []
    for dir in dirs:
	os.path.walk (dir, visit, list)
    
    return list

class ReadAutoDocs:
    def __init__ (self, archives, postFunctionCB=None):
	'''Read in all archives and build a new list of known libraries.

	    postFunctionCB - This is called when a new function has been
		    read.
	    dirs - A list of directories to examine.
	'''

	if not postFunctionCB:
	    postFunctionCB = lambda x, y: None
	self.postFunctionCB = postFunctionCB

	self.libs = {}

	for filename in archives:
	    self.processArchive (filename)

    sections = (
	'FUNCTION', 'RESULT', 'NOTES', 'EXAMPLE',
	'BUGS', 'SEE ALSO', 'INTERNALS', 'HISTORY'
    )

    def processArchive (self, filename):
	print 'Processing', filename

	dir = os.path.dirname (filename)
	lib = Library (os.path.join (dir))
	includes = []

	self.libs[lib.name] = lib

	func = None

	mode = 'searchheader'
	fh = open (filename, 'r')
	lineno = 0
	line = None
	while 1:
	    line = fh.readline ()
	    if not line: break
	    lineno = lineno + 1

	    if mode == 'searchheader':
		if line[:10] == '#Function ':
		    words = string.split (line)
		    rettype = string.join (words[1:-1])
		    funcname = words[-1]
		    func = Function (rettype, funcname)
		    func.includes = includes
		    lib.functions[func.name] = func
		elif line[:9] == '#Options':
		    words = string.split (line)
		    for word in words[1:]:
			func[flags] = None
		elif line[:10] == '#/Function':
		    self.postFunctionCB (self, lib, func)
		    func = None
		elif line[:11] == '#Parameter ':
		    words = string.split (line)
		    type, name = string.join (words[1:-2]), words[-2]
		    reg = words[-1]
		    parameter = Parameter (type, name, reg)
		    func.parameters.append (parameter)
		    mode = 'readparameter'
		elif line[:11] == '#LibOffset':
		    dummy, func.offset = string.split (line)
		elif line[:8] == '#AutoDoc':
		    # Now read the data in the header
		    mode = 'autodoc'
		    func.section = {}
		    # Clean autodoc section
		    for s in self.sections:
			func.section[s] = ''
		    currentsection = ''
		elif line[:7] == '#Header':
		    mode = 'readinc'
		    includes = []
		elif line[:5] == '#Code':
		    mode = 'readcode'
		    func.code = ''
		    includes = []
	    elif mode == 'readparameter':
		if line[:11] == '#/Parameter':
		    mode = 'searchheader'
		else:
		    line = string.strip (line)
		    parameter.explanation = parameter.explanation + line + '\n'
	    elif mode == 'readcode':
		if line[:6] == '#/Code':
		    mode = 'searchheader'
		else:
		    func.code = func.code + line + '\n'
	    elif mode == 'readinc':
		if line[:8] == '#/Header' or line[:6] == '#local':
		    mode = 'searchheader'
		elif line[:8] == '#include':
		    pos = string.find (line, '<')
		    if pos > 0:
			pos2 = string.find (line, '>', pos)
			file = line[pos+1:pos2]
			includes.append (file)
	    elif mode == 'autodoc':
		if line[:9] == '#/AutoDoc':
		    mode = 'searchheader'
		    continue

		line = string.strip (line)
		words = string.split (line)

		if len (words) == 1 and func.section.has_key (words[0]):
		    currentsection = words[0]
		elif len (words) == 2 and words == ['SEE', 'ALSO']:
		    currentsection = 'SEE ALSO'
		else:
		    assert (currentsection)
		    func.section[currentsection] = func.section[currentsection] + line + "\n"
		
	fh.close ()

class Library:
    longNameDB = {
	'clib': 'ANSI C link library',
	'alib': 'Amiga.lib',
	'devs': 'Devices',
	'aros': 'AROS',
	'arossupport': 'AROS Support',
	'intuition': 'Intuition',
	'exec': 'Exec',
	'boopsi': 'BOOPSI',
	'mathffp': 'Math FFP/Basic Functions',
	'mathtrans': 'Math FFP/Transient Functions',
	'mathieeesingbas': 'Math IEEE/Single Precision/Basic Functions',
	'mathieeedoubbas': 'Math IEEE/Double Precision/Basic Functions',
	'mathieeesingtrans': 'Math IEEE/Single Precision/Transient Functions',
	'mathieeedoubtrans': 'Math IEEE/Double Precision/Transient Functions',
    }

    def __init__ (self, dir):
	filename = os.path.join (dir, 'lib.conf')
	fh = open (filename, 'r')
	# Copy all settings into attributes of this object
	while 1:
	    line = fh.readline ()
	    if not line:
		break

	    words = string.split (string.rstrip (line), None, 1)
	    if words[0] == 'options':
		words[1] = string.split (words[1])

	    setattr (self, words[0], words[1])
	fh.close ()

	self.longName = self.longNameDB[self.name]
	self.functions = {}

    def __cmp__ (self, other):
	if not other:
	    return 1
	
	return cmp (self.longName, other.longName)

class Function:
    def __init__ (self, rettype, name):
	self.result = Parameter (rettype, '', '')
	self.name = name
	self.parameters = []
	self.section = {}

    def __cmp__ (self, other):
	if not other:
	    return 1
	
	return cmp (self.name, other.name)

class Parameter:
    def __init__ (self, type, name, registers):
	self.type, self.name = type, name
	self.registers = string.split (registers, '/')
	self.explanation = ''
