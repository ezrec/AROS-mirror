#
# File:     gcc.py
#
# Date:     2005/05/24
# Author:   Guillaume ROGUEZ <yomgui1@gmail.com>
#

import defaults

from utils import get_obj, execute, join_opts

CC = 'ppc-morphos-gcc'
CFLAGS = '-noixemul -Wall'
LFLAGS = '-noixemul'
LIB_LFLAGS = '-nostartfiles ' + LFLAGS
AR = 'ppc-morphos-ar'
ARFLAGS = 'cru'

def compile(target, source,
            cflags='',
            includes=None,
            env=defaults.DefaultEnv):
    if not target:
        target = get_obj(source)
        
    cmd = '%s -c -o %s %s %s %s' % (CC, target, cflags, join_opts(includes, '-I'), source)
    if env['quiet']:
        cmd += '>NIL:'
 
    env['log_fct'](target, pre="Compile '", post="'")
    if env['verbose']:
        print cmd
    
    return execute(cmd, env['dryrun'], env['quiet'])

def link(target, objs,
         lflags='',
         libs=None,
         env=defaults.DefaultEnv):
    if target and objs:
        if type(objs) != str:
            objs = ' '.join(objs)
            
        cmd = '%s -o %s %s %s %s' % (CC, target, lflags, objs, join_opts(libs, '-l')) 
        if env['quiet']:
            cmd += '>NIL:'

        env['log_fct'](target, pre="Link '", post="'")
        if env['verbose']:
            print cmd

        return execute(cmd, env['dryrun'], env['quiet'])
