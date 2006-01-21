# -*- coding: iso-8859-1 -*-
# Copyright © 2006, The AROS Development Team. All rights reserved.
# $Id$

# This script creates files in ReST format of the Library functions

# WARNING: before you use it you have to adopt the variables docdir1 and docdir2!

# TODO: cross references ("SEE ALSO")

import sys
import os
import string
import re

docdir1="/home/mazze/projects/AROS/workbench/libs"
docdir2="/home/mazze/projects/AROS/rom"
targetdir="../documentation/developers/autodocs"

# don't generate autodocs from this files
blacklist = ("buildeasyrequestargs.c", "buildeasyrequestargs_morphos.c",
		"buildsysrequest.c", "buildsysrequest_morphos.c",
		"refreshwindowframe.c", "refreshwindowframe_morphos.c",
		"setiprefs.c", "setiprefs_morphos.c",
		"sysreqhandler.c", "sysreqhandler_morphos.c",
		"match_old.c")

state_waitblock=1
state_inblock=2
state_name=3
#state_format=4
state_synopsis=5
state_location=6
state_function=7
state_inputs=8
state_result=9
state_example=10
state_notes=11
state_bugs=12
state_seealso=13
state_internals=14
state_history=15
state_template=16

class autodoc:
    def __init__(self):
        self.name=[]
        self.rettype = ""
        self.parameters=[]
        # self.format=[]
        self.synopsis=[]
        self.location=[]
        self.function=[]
        self.inputs=[]
        self.result=[]
        self.example=[]
        self.notes=[]
        self.bugs=[]
        self.seealso=[]
        self.internals=[]
        self.history=[]
        self.template=[]
        
    def __cmp__(self, other):
        return cmp(self.name, other.name)
        
    def read(self, filename):
        state=state_waitblock
        print "Reading " + filename
        f=open(filename, "r")
        while 1:
            line = f.readline()
            if line=="":
                break         
            
            line = string.expandtabs(line)
            
            if line[2:7]=="*****":
                if state==state_waitblock:
                    state=state_inblock
            
                elif state>state_inblock:    
                    break
                
            if state>=state_inblock:    
                if string.find(line, "NAME")>=0:
                    state=state_name
                #elif string.find(line, "FORMAT")>=0:
                    #state=state_format    
                elif string.find(line, "SYNOPSIS")>=0:
                    state=state_synopsis
                elif string.find(line, "LOCATION")>=0:
                    state=state_location    
                elif string.find(line, "FUNCTION")>=0:
                    state=state_function    
                elif string.find(line, "INPUTS")>=0:
                    state=state_inputs    
                elif string.find(line, "RESULT")>=0:
                    state=state_result    
                elif string.find(line, "EXAMPLE")>=0:
                    state=state_example    
                elif string.find(line, "NOTES")>=0:
                    state=state_notes    
                elif string.find(line, "BUGS")>=0:
                    state=state_bugs    
                elif string.find(line, "SEE ALSO")>=0:
                    state=state_seealso    
                elif string.find(line, "INTERNALS")>=0:
                    state=state_internals    
                elif string.find(line, "HISTORY")>=0:
                    state=state_history    
                elif string.find(line, "TEMPLATE")>=0:
                    state=state_template    

                elif state==state_name:
                    self.name.append(line)
                #elif state==state_format:
                #    self.format.append(line)
                elif state==state_synopsis:
                    self.synopsis.append(line)            
                elif state==state_location:
                    self.location.append(line)
                elif state==state_function:
                    self.function.append(line)
                elif state==state_inputs:
                    self.inputs.append(line)            
                elif state==state_result:
                    self.result.append(line)
                elif state==state_example:
                    self.example.append(line)
                elif state==state_notes:
                    self.notes.append(line)            
                elif state==state_bugs:
                    self.bugs.append(line)
                elif state==state_seealso:
                    self.seealso.append(line)
                elif state==state_internals:
                    self.internals.append(line)            
                elif state==state_history:
                    self.history.append(line)            
                elif state==state_template:
                    self.template.append(line)            

        f.close()

    def checkname(self):
        #scans the 'name' chapter for the name
        for line in self.name:
            m = pat.search(line)
            if m and len(m.groups()) == 2:
                self.name = m.group(2)
                self.rettype = m.group(1)
                return True
        return False
                                
    def getparameters(self):
        #scans parameters in synopsis chapter
        for line in self.synopsis:
            m = pat.search(line)
            if m and len(m.groups()) == 2:
                self.parameters.append( (m.group(1), m.group(2) ) )
            
    def printchapter(self, f, caption, content):
        # check for empty chapter
        nonempty = 0
        for line in content:
            if len(string.strip(line)) > 2:
                nonempty = 1
                break
                
        # don't print empty chapters
        if nonempty > 0:
            f.write(caption+"\n")
            f.write("~" * len(caption) + "\n")
            f.write("::\n\n")
            for line in content:
                if len(line)>4:
                    # one space to keep as literal block
                    f.write(" " + line[4:])
                else:
                    f.write("\n")    
            f.write("\n")
            
    def printout(self, f):
        f.write(self.name + "\n")
        f.write("=" * len(self.name) + "\n\n")
        
        f.write("Synopsis\n~~~~~~~~\n")
        f.write("::\n\n")
        f.write(" %s %s(\n" % (self.rettype, self.name))
        for linenr in range(len(self.parameters)):
            line = self.parameters[linenr]
            f.write("         %s %s" % (line[0], line[1]))
            if linenr < len(self.parameters) - 1:
                f.write(",")
            f.write("\n")    
        f.write(" );\n\n")    
        #self.printchapter(f, "Name", self.name)
        #self.printchapter(f, "Format", self.format)
        self.printchapter(f, "Template", self.template)
        #self.printchapter(f, "Synopsis", self.synopsis)
        #self.printchapter(f, "Location", self.location)
        self.printchapter(f, "Function", self.function)
        self.printchapter(f, "Inputs", self.inputs)
        self.printchapter(f, "Result", self.result)
        self.printchapter(f, "Example", self.example)
        self.printchapter(f, "Notes", self.notes)
        self.printchapter(f, "Bugs", self.bugs)
        self.printchapter(f, "See also", self.seealso)
        #self.printchapter(f, "Internals", self.internals)
        #self.printchapter(f, "history", self.history)
        f.write("\n")

# reads the .c files from a single directory
def createdocs(docdir, doclist):
        files=os.listdir(docdir)
        for docfile in files:
            if not (docfile in blacklist) and docfile[-2:]==".c":
                adoc=autodoc()
                adoc.read(docdir + "/" + docfile)
                if adoc.checkname():
                    doclist.append(adoc)
                adoc.getparameters()
                
# writes single autodoc file
def createdocfile(docdir, docname):
    doclist=[]
    createdocs(docdir, doclist)
    if len(doclist) > 0:
        doclist.sort()
	print "Writing " + docname
        f=open(targetdir + "/" + docname + ".en", "w")
        mark = "=" * len(docname)
        f.write(mark + "\n")
        f.write(docname + "\n")
        f.write(mark + "\n\n")

        f.write(".. This document is automatically generated. Don't edit it!\n\n")
        f.write("`Index <index>`_\n\n")
        f.write("----------\n\n")

        # create toc
        for doc in doclist:
            f.write("`%s`_\n" % doc.name)
        f.write("\n-----------\n\n")            
        
        # create the chapters
        for doc in doclist:
            doc.printout(f)
        f.close()

# writes all autodoc files
def makedoc(docdir):
    docdirlist = os.listdir(docdir)   # exec, graphics etc.
    for docname in docdirlist:
        if docname != ".svn" and os.path.isdir(docdir + "/" + docname):
            createdocfile(docdir + "/" + docname, docname)

pat = re.compile(r'AROS_.*\(\s*(.*?)\s*\,\s*(.*?)\s*\,')
makedoc(docdir1)
makedoc(docdir2)

# Create index
files = os.listdir(targetdir)
files.sort()

f = open(targetdir + "/index.en", "w")
print "Creating index.en"
f.write("======================\n")
f.write("Autodocs for Libraries\n")
f.write("======================\n\n")

for doc in files:
    if doc[0:5] != "index" and doc != ".svn":
        docname = doc[0:-3]    
        f.write("+ `%s <%s>`_\n" %(docname, docname))
f.close()    
