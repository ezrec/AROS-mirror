'''Convert paths in config.py into something more useful.'''

import config, os, sys

_varprog = None

# Copy of os.path.expandvars
def expandvars (path, dict=os.environ):
    """Expand shell variables of form $var and ${var}.  Unknown variables
are left unchanged"""
    global _varprog
    if '$' not in path:
        return path
    if not _varprog:
        import re
        _varprog = re.compile(r'\$(\w+|\{[^}]*\})')
    i = 0
    while 1:
        m = _varprog.search(path, i)
        if not m:
            break
        i, j = m.span(0)
        name = m.group(1)
        if name[:1] == '{' and name[-1:] == '}':
            name = name[1:-1]
        if dict.has_key(name):
            tail = path[j:]
            path = path[:i] + dict[name]
            i = len(path)
            path = path + tail
        else:
            i = j
    return path

def expandpath (path):
    return os.path.expanduser (expandvars (expandvars (path, config.__dict__)))

# Copy the "interesting" items of the config into local variables.
htmldir = expandpath (config.HTMLDIR)
ftpdir = expandpath (config.FTPDIR)
cvsrootdir = expandpath (config.CVSROOTDIR)
convert = config.__dict__.get ('CONVERT', None)
basedir = config.__dict__.get ('BASEDIR', None)
if not basedir:
    basedir = os.path.dirname (os.getcwd ())
basedir = os.path.abspath (basedir)
datadir = os.path.join (basedir, 'data')
sys.path.append (datadir)
lxrdir = os.path.join (basedir, 'lxr')

# Allow to read in python modules from AROS
arosdir = os.path.abspath (expandpath (config.AROSDIR))
path = os.path.join (arosdir, 'docs', 'src')
sys.path.append (path)


