# -*- coding: iso-8859-1 -*-
# Copyright © 2006, The AROS Development Team. All rights reserved.
# $Id$

# This script creates files in ReST format of the Shell commands

# WARNING: before you use it you have to adopt the variables docdir1 and docdir2!

# TODO: cross references ("SEE ALSO")

import sys
import os
import string

docdir1="/home/mazze/projects/AROS/workbench/c"
docdir2="/home/mazze/projects/AROS/workbench/c/shellcommands"
targetdir="../documentation/users/shell"

state_waitblock=1
state_inblock=2
state_name=3
state_format=4
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
        self.format=[]
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
        self.docname=""
        self.docfilename=""
        self.prevdocfilename=""
        self.nextdocfilename=""
        
    def __cmp__(self, other):
        return cmp(self.docfilename, other.docfilename)
        
    def read(self, filename):
        print "Reading file " + filename
        state=state_waitblock
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
                if line[4:8]=="NAME":
                    state=state_name
                elif line[4:10]=="FORMAT":
                    state=state_format    
                elif line[4:12]=="SYNOPSIS":
                    state=state_synopsis
                elif line[4:12]=="LOCATION":
                    state=state_location    
                elif line[4:12]=="FUNCTION":
                    state=state_function    
                elif line[4:10]=="INPUTS":
                    state=state_inputs    
                elif line[4:10]=="RESULT":
                    state=state_result    
                elif line[4:11]=="EXAMPLE":
                    state=state_example    
                elif line[4:9]=="NOTES":
                    state=state_notes    
                elif line[4:8]=="BUGS":
                    state=state_bugs    
                elif line[4:12]=="SEE ALSO":
                    state=state_seealso    
                elif line[4:13]=="INTERNALS":
                    state=state_internals    
                elif line[4:11]=="HISTORY":
                    state=state_history    
                elif line[4:12]=="TEMPLATE":
                    state=state_template    

                elif state==state_name:
                    self.name.append(line)
                elif state==state_format:
                    self.format.append(line)
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
            part=string.split(line)
            if len(part) > 0 and len(part[0]) > 1:
                self.docname = part[0]
                self.docfilename = string.lower(self.docname)
                return True
        return False
                                
    def printchapter(self, f, caption, content):
        # check for empty chapter
        nonempty = 0
        for line in content:
            if line != "\n":
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
            
    def printout(self):
        targetfile = targetdir + "/" + self.docfilename+".en" 
        print "Creating file " + targetfile
        f=open(targetfile, "w")
        f.write(".. This document is automatically generated. Don't edit it!\n\n")
        f.write("`Index <index>`_ ")
        if self.prevdocfilename:
            f.write("`Prev <" + self.prevdocfilename + ">`_ ")
        if self.nextdocfilename:
            f.write("`Next <" + self.nextdocfilename + ">`_ ")
                
        f.write("\n\n---------------\n")
        marks="=" * len(self.docname) + "\n"
        f.write("\n\n" + marks)
        f.write(self.docname + "\n")
        f.write(marks + "\n")
        self.printchapter(f, "Format", self.format)
        self.printchapter(f, "Template", self.template)
        self.printchapter(f, "Synopsis", self.synopsis)
        self.printchapter(f, "Location", self.location)
        self.printchapter(f, "Function", self.function)
        self.printchapter(f, "Inputs", self.inputs)
        self.printchapter(f, "Result", self.result)
        self.printchapter(f, "Example", self.example)
        self.printchapter(f, "Notes", self.notes)
        self.printchapter(f, "Bugs", self.bugs)
        self.printchapter(f, "See also", self.seealso)
        #self.printchapter(f, "Internals", self.internals)
        #self.printchapter(f, "history", self.history)
        f.close()
        


def createdocs(docdir):
        files=os.listdir(docdir)
        for docfile in files:
            if docfile[-2:]==".c":
                adoc=autodoc()
                adoc.read(docdir + "/" + docfile)
                if adoc.checkname():
                    doclist.append(adoc)

doclist=[]
createdocs(docdir1)
createdocs(docdir2)
doclist.sort()

# connect the capter files
for docnr in range(len(doclist)):
    if docnr>0:
        doclist[docnr].prevdocfilename = doclist[docnr - 1].docfilename
    if docnr<len(doclist) - 1:
        doclist[docnr].nextdocfilename = doclist[docnr + 1].docfilename
                        
# create the capter files
for doc in doclist:
    doc.printout()

# create index page
print "Creating index file"
f=open(targetdir + "/index.en", "w")
f.write(".. This document is automatically generated. Don't edit it!\n\n")
f.write("==============\n")
f.write("Shell commands\n")
f.write("==============\n\n")

f.write("`Introduction <introduction>`_\n\n")

for doc in doclist:
    f.write("`" + doc.docname + "`__\n")
f.write("\n")
for doc in doclist:    
    f.write("__ " + doc.docfilename + "\n")
f.close()
print "Done"
