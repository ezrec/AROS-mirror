#
# File:     utils.py
#
# Date:     2005/05/24
# Author:   Guillaume ROGUEZ <yomgui1@gmail.com>
#

import os
import sys
from defaults import DefaultEnv

exists = os.path.exists
getmtime = os.path.getmtime

__all__ = ['get_obj', 'get_src', 'checkDepends', 'checkDepend',
           'log', 'execute', 'flexcat', 'flexcat_header', 'join_opts', 'fromposix']

get_obj = lambda n: os.path.splitext(n)[0]+'.o'
get_src = lambda n: os.path.splitext(n)[0]+'.c'

class checkTime:
    def __init__(self, file):
        self.time = getmtime(file)

    def __call__(self, file):
        return not exists(file) or (getmtime(file) > self.time)
  
def checkDepends(target, dep_list):
    return (not exists(target) and dep_list) or filter(checkTime(target), dep_list)

def checkDepend(target, dep_file):
    return (not exists(target) and dep_file) or checkTime(target)(dep_file)

def log(x, pre='', post=''):
    print '>> %s[1m%s[0m%s' % (pre, x,post)

def execute(cmd, dry=False, quiet=False):
    if not dry:
        if quiet:
            return os.system(cmd+' >NIL:')
        else:
            return os.system(cmd)
    return 0

def flexcat(target, cd, ct, env=DefaultEnv):
    if not env['quiet']:
        log(target, pre="Generate '", post="'")
    
    cmd = 'flexcat "%s" "%s" CATALOG "%s"' % (cd, ct, target)
    if env['verbose']:
        print cmd

    return execute(cmd, env['dryrun'], env['quiet'])

def flexcat_header(name, cd, sd, env=DefaultEnv):
    if  not env['quiet']:
        env['log_fct'](name, pre="Generate '", post="'")
    
    cmd = 'flexcat "%s" "%s=%s"' % (cd, name, sd)
    if env['verbose']:
        print cmd

    return execute(cmd, env['dryrun'], env['quiet'])

def join_opts(opts, pre=''):
    return ' '.join([pre+x for x in opts])

import posixpath

if os.name == 'morphos':
    def fromposix(path):
        """Convert a posix path into a native path."""

        path = posixpath.normpath(path)

        if path and path[0] == '/':
            path = path[1:]
            if path:
                try:
                    i = path.index('/')
                except ValueError:
                    i = len(path)

                path = path[:i] + ':' + path[i+1:]
            else:
                raise ValueError, "path not found"

        if path == '.':
            return ''

        return path.replace('../', '/').replace('..', '/')

else: # assume that native paths are in a posix form
    def fromposix(path):
        return path
