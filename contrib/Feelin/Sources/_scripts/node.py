import os, Builder
import posixpath as _path

from utils import *
from defaults import DefaultEnv

class Node:
    """Nodes represent any individual element that should be handled.
    A node hasn't a 'position'. It's not a file with a pathname for example.
    So, two node with same name may be exist. Only the object existance identifies a node.
    Take care of this rule.
    A node may result of a build process. This last uses others nodes (subnodes) and
    a builder to create a 'processed' node.
    A node known how it can be generated and by which subnodes. So, its subnodes and builder
    are dependencies of a node.
    """

    def __init__(self, name, subnodes=[], builder=None, env=DefaultEnv):
        self.name = name
        self.snodes = []
        self += subnodes
        self.builder = builder
        self.env = env
        self.body = None            # a node doesn't exist until it is built
        self.snode_changed = []
        self.modified = False

    def __str__(self):
        return self.name

    def __iadd__(self, subnodes):
        """Add a list or one nodes to the list of subnodes."""
        
        if isinstance(subnodes, Node):
            self.snodes.append(subnodes)
            self.modified = True
        else:
            subnodes = filter(lambda x: isinstance(x, Node), subnodes)
            self.snodes.extend(subnodes)
            self.modified = subnodes != []
        
        return self

    def build(self):
        """Apply owned builder to subnodes"""
        
        if self.builder:
            dep = self.check_dep()
            for node in dep:
                node.build()

            if self.snode_changed or self.changed():
                self.body = self.builder(self)
        
        else:
            raise Builder.BuildError(self, "no builder found for this node")

    def check_dep(self):
        """Check if subnodes or builder have changed.
        Return a list of changed subnodes.
        """

        del self.snode_changed

        # builder changed => all subnodes must be rebuilt
        if self.builder and self.builder.changed():
            self.snode_changed = [].expand(self.snodes)
        else:
            self.snode_changed = filter(lambda x: x.changed() or x.check_dep(), self.snodes)

        self.touch()
        return self.snode_changed
    
    def changed(self):
        """Return True if node himself needs to be build."""
        
        return self.modified or not self.body

    def touch(self):
        pass


class Dir(Node):
    def __init__(self, path, builder=Builder.MakeDir(), env=DefaultEnv, **kw):
        Node.__init__(self, path, builder=builder, env=env, **kw)
        if _path.isabs(path):
            self.root = ''
        else:
            self.root = env['root']

        self.set_root(self.root)

    def changed(self):
        self.modified = not os.path.exists(self.native_abspath)
        return self.modified

    def get_native_abspath(self):
        native_path = fromposix(self.name)
        if os.path.isabs(native_path):
            self.native_abspath = native_path
        else:
            self.native_abspath = os.path.join(fromposix(self.root), native_path)
        return self.native_abspath

    def set_root(self, root):
        self.root = root
        self.get_native_abspath()


from time import time as _time

class File(Node):
    def __init__(self, name, subnodes=[], builder=Builder.Exist(), **kw):
        path, self.filename = _path.split(name)
        if not self.filename:
            raise ArgumentError, "no filename"

        Node.__init__(self, name, subnodes=subnodes, builder=builder, **kw)
        
        self.dir = Dir(path, **kw)
        self += self.dir
        self.get_native_fullname()
        if os.path.exists(self.fullname):
            self.mtime = os.stat(self.fullname).st_mtime
        else:
            self.mtime = 0

    def changed(self):
        return (not os.path.exists(self.fullname)) or (os.stat(self.fullname).st_mtime > self.mtime)

    def touch(self):
        self.mtime = _time()

    def get_native_fullname(self):
        if not self.__dict__.has_key('fullname'):
            self.fullname = os.path.join(self.dir.native_abspath, self.name)
        return self.fullname

    def set_root(self, root):
        self.dir.set_root(root)
        self.get_native_fullname()

def get_builder(name):
    return Builder.Builder()

class Program(File):
    def __init__(self, target, sources=None, **kw):
        if not sources:
            sources = target
            target = None

        sources = self.add_sources(sources, **kw)

        if not target:
            target = _path.splitext(str(sources[0]))[0]

        File.__init__(self, name=target, subnodes=sources, builder=get_builder(target), **kw)

    def add_sources(self, sources, nodes=[], **kw):
        if isinstance(sources, str):
            nodes.extend(map(lambda x: File(name=x, **kw), sources.split()))
        elif isinstance(sources, Node):
            nodes.append(sources)
        else:
            list = tuple(sources)
            sources = []
            for source in list:
                self.add_sources(source, nodes, **kw)

        return nodes

def print_dep_tree(root, level=0):
    print ' '*level + "%s %s" % (root.changed() and '*' or '', str(root) or '<curdir>')
    level += 3
    for node in root.snodes:
        print_dep_tree(node, level)
