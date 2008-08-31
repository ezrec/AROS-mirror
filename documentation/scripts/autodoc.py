# -*- coding: iso-8859-1 -*-
# Copyright © 2008, The AROS Development Team. All rights reserved.
# $Id$

# This script creates files in ReST format from autodoc headers

# TODO:
# - Cross references
# - Parse static libraries


import glob
import os
import re

# Relative path from the main build script to the root of the AROS source
topdir = ".."

# Regex for autodoc block (without surrounding comments)
ad_regx = re.compile(r"""
^/\*{6,}
(
.*?
^[/*\s]{4,4}NAME
.*?
)
^\*{7,}/
""", re.VERBOSE | re.MULTILINE | re.DOTALL)

# Regex for a title
titles_regx = re.compile(r"""
^[/\*\s]{4,4}
(NAME|FORMAT|SYNOPSIS|LOCATION|FUNCTION|INPUTS|RESULT|EXAMPLE|NOTES|BUGS|SEE\ ALSO|INTERNALS|HISTORY|TEMPLATE)[/\*\s]*?$
""", re.VERBOSE)

# Regex for AROS library functions
libfunc_regx = re.compile(r'AROS_.*\(\s*(.*?)\s*\,\s*(.*?)\s*\,')

# don't generate autodocs from this files
blacklist = ("buildeasyrequestargs.c", "buildeasyrequestargs_morphos.c",
		"buildsysrequest.c", "buildsysrequest_morphos.c",
		"refreshwindowframe.c", "refreshwindowframe_morphos.c",
		"setiprefs.c", "setiprefs_morphos.c",
		"sysreqhandler.c", "sysreqhandler_morphos.c",
		"match_old.c")


class autodoc:
    def __init__(self, content):
        self.titles = {} # dict for each title
        self.docname = "" # function or command name
        self.docfilename = "" # filename without ".en"

        current_title = None
        for line in content.splitlines():
            match = titles_regx.match(line)
            if match:
                current_title = match.group(1)
                self.titles[current_title] = ""
            elif current_title:
                self.titles[current_title] += (" " + line.expandtabs())[4:] + "\n"
        
        # check for empty chapters, because we don't want to print them
        for title, content in self.titles.iteritems():
            if content.strip() == "":
                self.titles[title] = ""

    def __cmp__(self, other):
        return cmp(self.docfilename, other.docfilename)

    def write(self, filehandle, titles):
        for title in titles:
            title_key = title.upper()
            if self.titles.has_key(title_key):
                lines = self.titles[title_key]
                if len(lines) > 0:
                    filehandle.write(title + "\n")
                    filehandle.write("~" * len(title) + "\n")
                    filehandle.write("::\n\n")
                    filehandle.write(lines)
                    filehandle.write("\n")


class shellautodoc(autodoc):
    def __init__(self, content):
        autodoc.__init__(self, content)
        self.prevdocfilename = ""
        self.nextdocfilename = ""

        # get docname
        self.docname = self.titles["NAME"].split()[0]
        if self.docname == "":
            raise ValueError("docname is empty")
        self.docfilename = self.docname.lower()

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
        
        autodoc.write(self, filehandle, titles)


class libautodoc(autodoc):
    def __init__(self, content):
        autodoc.__init__(self, content)
        self.rettype = ""
        self.parameters = []

        # search for function name
        m = libfunc_regx.search(content)
        if m:
            self.docname = m.group(2)
            self.docfilename = self.docname.lower()
            self.rettype = m.group(1)
        else:
            return
        
        #search for parameter/type
        if self.titles.has_key("SYNOPSIS"):
            for par in libfunc_regx.findall(self.titles["SYNOPSIS"]):
                self.parameters.append(par)

        # create new Synopsis
        syn = " " + self.rettype + " " + self.docname
        if len(self.parameters) == 0:
            syn += "();\n"
        else:
            syn += "(\n"
            for line in self.parameters:
                syn += "          " + line[0] + " " + line[1]
                if line is self.parameters[-1]:
                    syn += " );\n"
                else:
                    syn += ",\n"
        
        # check for variadic prototype
        varproto = ""
        if len(self.parameters) > 0:
            if self.docname[-1] == "A":
                # function name ends with "A"
                varproto = self.docname[:-1]
            elif self.docname[-7:] == "TagList":
                # function name ends with "TagList"
                varproto = self.docname[:-7] + "Tags"
            elif self.docname[-4:] == "Args" and (self.docname not in ("ReadArgs","FreeArgs")):
                # function name ends with "Args"
                varproto = self.docname[:-4]
            else:
                # last argument's type is "const struct TagItem *"
                lastarg = self.parameters[-1] # last parameter
                lastarg = lastarg[0] # type
                if lastarg[-16:] == "struct TagItem *":
                    varproto = self.docname + "Tags"
            
        # append variadic prototype
        if len(varproto) > 0:
            syn += " \n"
            syn += " " + self.rettype + " " + varproto + "(\n"
            if len(self.parameters) > 1:
                for line in self.parameters[:-1]:
                    syn += "          " + line[0] + " " + line[1] + ",\n"
            syn += "          TAG tag, ... );\n"

        self.titles["SYNOPSIS"] = syn

    def write(self, filehandle, titles):
        filehandle.write(self.docname + "\n")
        filehandle.write("=" * len(self.docname) + "\n\n")
        autodoc.write(self, filehandle, titles)
        filehandle.write("\n")


class shelldoclist:
    def __init__(self):
        self.doclist = []

    def read(self, srcdir):
        filenames = glob.glob(os.path.join(srcdir, "*.c"))
        for filename in filenames:
            print "Reading from file", filename
            filehandle = open(filename)
            content = filehandle.read() # read whole file
            for ad_entry in ad_regx.findall(content):
                ad = shellautodoc(ad_entry)
                if ad.docname != "":
                    self.doclist.append(ad)
            filehandle.close()
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
        filehandle.write("Shell Commands\n")
        filehandle.write("==============\n\n")
        filehandle.write("`Introduction <introduction>`_\n\n")

        for doc in self.doclist:
            filehandle.write("`" + doc.docname + "`__\n")
        filehandle.write("\n")
        for doc in self.doclist:    
            filehandle.write("__ " + doc.docfilename + "\n")

    def write(self, targetdir, titles):
        for doc in self.doclist:
            filename = os.path.join(targetdir, doc.docfilename + ".en")
            print "Writing to file", filename
            filehandle = open(filename, "w")
            doc.write(filehandle, titles)
            filehandle.close()
        
        # create index page
        filehandle = open(os.path.join(targetdir, "index.en"), "w")
        self.write_index(filehandle)
        filehandle.close()


class libdoclist:
    def __init__(self):
        self.doclist = []
        self.docfilename = ""

    def read(self, srcdir):
        self.docfilename = os.path.basename(srcdir) # rightmost part of the path
        cfiles = os.listdir(srcdir)
        for file in cfiles:
            if not (file in blacklist) and file[-2:]==".c":
                filename = os.path.join(srcdir, file)
                print "Reading from file", filename
                filehandle = open(filename)
                content = filehandle.read() # read whole file
                for ad_entry in ad_regx.findall(content):
                    ad = libautodoc(ad_entry)
                    if ad.docname != "":
                        self.doclist.append(ad)
                filehandle.close()
        self.doclist.sort()

    def write(self, targetdir, titles):
        if len(self.doclist) > 0:
            filename = os.path.join(targetdir, self.docfilename + ".en")
            print "Writing to file", filename
            filehandle = open(filename, "w")

            #create header
            underline = "=" * len(self.docfilename)
            filehandle.write(underline + "\n")
            filehandle.write(self.docfilename + "\n")
            filehandle.write(underline + "\n\n")

            filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")
            filehandle.write("`Index <index>`_\n\n")
            filehandle.write("----------\n\n")

            # create toc
            tablesep = (("=" * 39) + " ") * 4
            filehandle.write(tablesep + "\n")
            for docnr in range(len(self.doclist)):
                tocname = "`" + self.doclist[docnr].docname + "`_"
                tocname = tocname.ljust(40)
                filehandle.write(tocname)
                if (docnr + 1) % 4 == 0:
                    filehandle.write("\n")
            filehandle.write("\n" + tablesep)
            filehandle.write("\n\n-----------\n\n")            
     
            for doc in self.doclist:
                doc.write(filehandle, titles)
            filehandle.close()


def create_lib_docs():
    targetdir = os.path.join("documentation", "test")
    targetdir = os.path.join("documentation", "developers", "autodocs")
    srcdirs = ( os.path.join(topdir, "rom"), os.path.join(topdir, "workbench/libs") )
    for dir in srcdirs:
        create_lib_docs_dir(dir, targetdir)

    # print index file
    files = os.listdir(targetdir)
    files.sort()

    filehandle = open(os.path.join(targetdir, "index.en"), "w")
    print "Creating index.en"
    filehandle.write("======================\n")
    filehandle.write("Autodocs for Libraries\n")
    filehandle.write("======================\n\n")
    filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")

    for doc in files:
        if doc[0:5] != "index" and doc != ".svn":
            docname = doc[0:-3]
            filehandle.write("+ `%s <%s>`_\n" %(docname, docname))
    filehandle.close()
    

def create_lib_docs_dir(srcdir, targetdir):
    subdirs = os.listdir(srcdir)
    lib_titles = ("Synopsis","Template","Function",
        "Inputs","Result","Example","Notes","Bugs","See also")  # The titles we want
                                                                # to be print
    for dir in subdirs: # exec ,graphics etc.
        docpath = os.path.join(srcdir, dir)
        if (dir != ".svn") and os.path.isdir(docpath) :
            libdocs = libdoclist()
            libdocs.read(docpath)
            libdocs.write(targetdir, lib_titles)

def create_shell_docs():
    srcdirs = ( os.path.join(topdir, "workbench", "c"),
                os.path.join(topdir, "workbench", "c", "shellcommands"),
                os.path.join(topdir, "workbench", "c", "Identify"),
                os.path.join(topdir, "workbench", "c", "iprefs") )
                
    targetdir = os.path.join("documentation", "users", "shell") # relative to main build script
    shell_titles = ("Name","Format","Template","Synopsis","Location","Function",
        "Inputs","Result","Example","Notes","Bugs","See also")  # The titles we want
                                                                # to be print
    shelldocs = shelldoclist()
    for dir in srcdirs:
        shelldocs.read(dir)
    shelldocs.write(targetdir, shell_titles)
    print "Done"

def create_all_docs():
    create_shell_docs()
    create_lib_docs()


if __name__ == "__main__":
    os.chdir("..") # because we have relative paths
    create_all_docs()
