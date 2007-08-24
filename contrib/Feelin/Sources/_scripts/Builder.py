import os.path
from utils import fromposix

class BuildError(Exception):
    def __init__(self, node, cause):
        self.node = node
        self.cause = cause

    def __str__(self):
        return "Build error for '%s': %s" % (self.node, self.cause)

class Builder:
    def __call__(self, node):
        print "building "+str(node)+" with", map(str, node.snodes)
        return None

    def changed(self):
        return False

class MakeDir(Builder):
    def __call__(self, node):
        print "making directory '%s'" % fromposix(str(node))
        return None

class Exist(Builder):
    def __call__(self, node):
        target = node.get_native_fullname()
        if os.path.exists(target):
            return target
        else:
            raise BuildError(target, "file doesn't exist and I don't know how to create it")
