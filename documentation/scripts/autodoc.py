# -*- coding: iso-8859-1 -*-
# Copyright © 2008, The AROS Development Team. All rights reserved.
# $Id$

# This script creates files in ReST format from autodoc headers

import glob

# Relative path from the main build script to the root of the AROS source
topdir = ".."

# All defined autodoc titles
titles = ("NAME","FORMAT","SYNOPSIS","LOCATION","FUNCTION","INPUTS","RESULT","EXAMPLE",
    "NOTES","BUGS","SEE ALSO","INTERNALS","HISTORY","TEMPLATE")

state_waitblock=1
state_inblock=2
state_intitle=3


class autodoc:

    def __init__(self):
        self.reset()


    def __cmp__(self, other):
        return cmp(self.docfilename, other.docfilename)


    def reset(self):
        self.titles = {} # dict with table with strings for each title
        for title in titles:
            self.titles[title] = []
        self.docname = "" # function or command name
        self.docfilename = "" # filename without ".en"
        self.prevdocfilename = ""
        self.nextdocfilename = ""

    def read(self, filehandle):
        self.reset()
        state = state_waitblock
        while 1:
            line = filehandle.readline().expandtabs()
            if line == "":
                # end of file
                return False
            if state == state_waitblock:
                if line[:7] == "/******":
                    state = state_inblock
            elif state == state_inblock:
                if line[:7] == "*******":
                    # we've reached the end marker without any title
                    self.reset()
                    state = state_waitblock
                elif line.strip() in titles:
                    state = state_intitle
                    current_title = line.strip()
            elif state == state_intitle:
                if line[:7] == "*******":
                    break
                elif line.strip() in titles:
                    state = state_intitle
                    current_title = line.strip()
                else:
                    self.titles[current_title].append(line)
        
        # check for empty chapters, because we don't want to print them
        # replace them by an empty list
        for title in self.titles:
            content = ""
            for line in self.titles[title]:
                content = content + line
            content = content.strip()
            if content == "":
                self.titles[title] = []
                
        # get docname
        for line in self.titles["NAME"]:
            self.docname = self.docname + line
        self.docname = self.docname.split()[0]
        if self.docname == "":
            raise ValueError("docname is empty")
        self.docfilename = self.docname.lower()
        
        return True


    def write(self, filehandle, titles):
        underline="=" * len(self.docname) + "\n"
        filehandle.write(underline)
        filehandle.write(self.docname + "\n")
        filehandle.write(underline + "\n")

        filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")

        filehandle.write("`Index <index>`_ ")
        if self.prevdocfilename:
            filehandle.write("`Prev <" + self.prevdocfilename + ">`_ ")
        if self.nextdocfilename:
            filehandle.write("`Next <" + self.nextdocfilename + ">`_ ")

        filehandle.write("\n\n---------------\n\n")

        for title in titles:
            title_key = title.upper()
            if self.titles.has_key(title_key):
                if len(self.titles[title_key]) > 0:
                    filehandle.write(title + "\n")
                    filehandle.write("~" * len(title) + "\n")
                    filehandle.write("::\n\n")
                    for line in self.titles[title_key]:
                        if len(line)>4:
                            # one space to keep as literal block
                            filehandle.write(" " + line[4:])
                        else:
                            filehandle.write("\n")    
                    filehandle.write("\n")


class shelldoclist:

    def __init__(self):
        self.doclist = []
        self.targetdir = "."


    def read(self, srcdir):
        filenames = glob.glob(srcdir + "/*.c")
        for filename in filenames:
            print "Reading from file", filename
            file = open(filename)
            ad = autodoc()
            ad.read(file)
            if ad.docname != "":
                self.doclist.append(ad)
            file.close()
        self.doclist.sort()

        # set prev/next fields
        for docnr in range(len(self.doclist)):
            if docnr>0:
                self.doclist[docnr].prevdocfilename = self.doclist[docnr - 1].docfilename
            if docnr<len(self.doclist) - 1:
                self.doclist[docnr].nextdocfilename = self.doclist[docnr + 1].docfilename


    def write_index(self, filehandle):
        print "Creating index file"
        filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")
        filehandle.write("==============\n")
        filehandle.write("Shell commands\n")
        filehandle.write("==============\n\n")
        filehandle.write("`Introduction <introduction>`_\n\n")

        for doc in self.doclist:
            filehandle.write("`" + doc.docname + "`__\n")
        filehandle.write("\n")
        for doc in self.doclist:    
            filehandle.write("__ " + doc.docfilename + "\n")
        print "Done"


    def write(self, targetdir, titles):
        for doc in self.doclist:
            filename = targetdir + "/" + doc.docfilename + ".en"
            print "Writing to file", filename
            filehandle = open(filename, "w")
            doc.write(filehandle, titles)
            filehandle.close()
        
        # create index page
        filehandle = open(targetdir + "/index.en", "w")
        self.write_index(filehandle)
        filehandle.close()

def create_lib_docs():
    pass

def create_shell_docs():
    srcdirs = ( topdir + "/workbench/c",
                topdir + "/workbench/c/shellcommands",
                topdir + "/workbench/c/Identity",
                topdir + "/workbench/c/iprefs" )
                
    targetdir = "documentation/users/shell" # relative to main build script
    shell_titles = ("Name","Format","Template","Synopsis","Location","Function",
        "Inputs","Result","Example","Notes","Bugs","See also") #  The titles we want
                                                                # to be print
    shelldocs = shelldoclist()
    for dir in srcdirs:
        shelldocs.read(dir)
    shelldocs.write(targetdir, shell_titles)

def create_all_docs():
    create_shell_docs()
