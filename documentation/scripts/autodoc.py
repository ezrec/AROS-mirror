# -*- coding: iso-8859-1 -*-
# Copyright © 2008-2010, The AROS Development Team. All rights reserved.
# $Id$


"""Autodoc to ReST converter.

Classes and functions for parsing autodoc headers and writing them
in ReST format.

For that the script can find the sources the documentation must be checked out like
this to the AROS directory:

AROS
    ...
    compiler
    workbench
    rom
    ...
    documentation
        scripts
...

If you have them in another directory the variable "topdir" can be adjusted.
If you use a relative path take care that the script is called by the main build script
from one directory level above.
"""

# TODO:
# - handle classes which are embedded in libraries
# - fix xref of HIDDs
# - xref links to function macros
# - differ between libraries, devices and linklibs

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
(NAME|FORMAT|SYNOPSIS|LOCATION|FUNCTION|INPUTS|TAGS|RESULT|EXAMPLE|NOTES|BUGS|SEE\ ALSO|INTERNALS|HISTORY|TEMPLATE)[/\*\s]*?$
""", re.VERBOSE)

# Regex for AROS library functions
libfunc_regx = re.compile(r'AROS_.*\(\s*(.*?)\s*\,\s*(.*?)\s*\,')

# Regex for C library functions
cfunc_regx = re.compile(r"^\s*(.*?)(\w*)\s*\([\w,()]*$", re.MULTILINE)

# Regex for splitting a block by comma and \n
split_regx = re.compile(r"[\n,]+", re.MULTILINE)

# Regex for parsing a line of "see also"
xref_regx = re.compile(r"""
^
\s*<*
(
(?P<libname>\w+?)\.library/(?P<funcname>\w+?)\(\)
|
(?P<localfuncname>\w+?)\(\)
|
(?P<header>[\w/-]+\.h)
|
(?P<command>[\w-]+)
|
(?P<path>.+?)
)
>*\s*
$
""", re.VERBOSE | re.MULTILINE)

xref_kind_function = 1
xref_kind_localfunction = 2
xref_kind_header = 3
xref_kind_string = 4
xref_kind_any = 5

# don't generate autodocs from these files
blacklist = (   "refreshwindowframe.c",
                "setiprefs.c",
                "match_old.c","dosdoio.c","exec_util.c","strerror_rom.c",
                "runprocess.c", "monitorclass.c", "basemetaclass.c", "rootclass.c")


class autodoc:
    """ Autodoc base class.
    
    Subclasses must set the elements
    docname (function or command name e.g. "Draw") and docfilename (lower case docname)
    """
    
    def __init__(self, content):
        """Constructor. Reads and parses autodoc string.

        Arguments:

        content - String of autodoc text chunk without sourrounding comment lines
        """
        
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
        
        # parse "see also"
        if self.titles.has_key("SEE ALSO"):
            self.titles["XREF"] = []
            for ref in split_regx.split(self.titles["SEE ALSO"]):
                xref = xref_regx.match(ref)
                if xref:
                    libname = xref.group('libname')
                    funcname = xref.group('funcname')
                    localfuncname = xref.group('localfuncname')
                    path = xref.group('path')
                    command = xref.group('command')
                    header = xref.group('header')
                    # check for allowed combinations
                    if libname and funcname and not localfuncname and not path and not command and not header:
                        # libname + funcname
                        self.titles["XREF"].append( (xref_kind_function, libname, funcname) )
                    elif not libname and not funcname and localfuncname and not path and not command and not header:
                        # localfuncname
                        self.titles["XREF"].append( (xref_kind_localfunction, localfuncname, "") )
                    elif not libname and not funcname and not localfuncname and path and not command and not header:
                        # path
                        self.titles["XREF"].append( (xref_kind_any, path, "") )
                    elif not libname and not funcname and not localfuncname and not path and command and not header:
                        # command
                        self.titles["XREF"].append( (xref_kind_string, command, "") )
                    elif not libname and not funcname and not localfuncname and not path and not command and header:
                        # header
                        self.titles["XREF"].append( (xref_kind_header, header, "") )
                    else:
                        print "*" * 20
                        print self.titles["SEE ALSO"]
                        raise ValueError("XREF parsing error")
        
    def __cmp__(self, other):
        """Compare function for sorting by docfilename.
        """
        
        return cmp(self.docfilename, other.docfilename)

    def write(self, filehandle, titles):
        """Write autodoc elements to file.
        
        Arguments:
        
        filehandle - filehandle of a file to write the autodoc in
        titles - what titles (e.g. Synopsis, Note) should be printed
        """
        
        for title in titles:
            title_key = title.upper()
            if title_key != "SEE ALSO" and self.titles.has_key(title_key):
                lines = self.titles[title_key]
                if len(lines) > 0:
                    filehandle.write(title + "\n")
                    filehandle.write("~" * len(title) + "\n")
                    filehandle.write("::\n\n")
                    filehandle.write(lines)
                    filehandle.write("\n")

    def write_xref(self, filehandle, path_to_shell, path_to_lib, path_to_header):
        """Write xrefs ('see also') to file.
        
        Arguments:
        
        filehandle - filehandle of a file to write the autodoc in
        path_to_shell - relative path from target document to directory with Shell command documents
        path_to_lib - relative path from target document to directory with library documents
        path_to_header - relative path from target document to directory with header files
        """
        
        if self.titles.has_key("XREF"):
            if len(self.titles["XREF"]) > 0:
                filehandle.write("See also\n~~~~~~~~\n\n")
                for kind, name1, name2 in self.titles["XREF"]:
                    if kind == xref_kind_function:
                        self.write_xref_function(filehandle, name1, name2, path_to_lib)
                    elif kind == xref_kind_localfunction:
                        self.write_xref_localfunction(filehandle, name1)
                    elif kind == xref_kind_any:
                        self.write_xref_any(filehandle, name1)
                    elif kind == xref_kind_string:
                        self.write_xref_string(filehandle, name1)
                    elif kind == xref_kind_header:
                        self.write_xref_header(filehandle, name1, path_to_header)
                filehandle.write("\n\n")

    def write_xref_function(self, filehandle, libname, funcname, path_to_lib):
        filehandle.write("`%s.library/%s() <%s/%s#%s>`_ "
                        %(libname, funcname, path_to_lib, libname, funcname.lower()) )

    def write_xref_localfunction(self, filehandle, funcname):
        filehandle.write("`%s()`_ " %(funcname) )

    def write_xref_header(self, filehandle, name, path):
        filehandle.write("`%s <%s/%s>`_ " %(name, path, name) )
    
    def write_xref_string(self, filehandle, name):
        filehandle.write("`%s`_ " %(name) )

    def write_xref_any(self, filehandle, name):
        filehandle.write("%s " %name)                  


class shellautodoc(autodoc):
    """Autodoc class for Shell commands and applications.
    """
    
    def __init__(self, content):
        """Constructor. Reads and parses autodoc string.

        Arguments:

        content - String of autodoc text chunk without sourrounding comment lines
        """

        autodoc.__init__(self, content)
        self.prevdocfilename = ""
        self.nextdocfilename = ""

        # get docname
        self.docname = self.titles["NAME"].split()[0]
        if self.docname == "":
            raise ValueError("docname is empty")
        self.docfilename = self.docname.lower()

    def write(self, filehandle, titles):
        """Write autodoc elements to file.
        
        Arguments:
        
        filehandle -    filehandle of a file to write the autodoc in
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.

        """

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

    def write_xref_string(self, filehandle, name):
        filehandle.write("`%s <%s>`_ " %(name, name.lower()) )


class libautodoc(autodoc):
    """Autodoc class for library functions (shared and static).
    """

    def __init__(self, content):
        """Constructor. Reads and parses autodoc string.

        Arguments:

        content - String of autodoc text chunk without sourrounding comment lines
        """

        autodoc.__init__(self, content)
        self.rettype = ""
        self.parameters = []

        # search for function name
        m = libfunc_regx.search(content)
        if m:
            # AROS lib function
            self.docname = m.group(2)
            self.rettype = m.group(1)
            self.docfilename = self.docname.lower()

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
        
        else:
            # C function
            m = cfunc_regx.search(content)
            if m:
                self.docname = m.group(2)
                self.rettype = m.group(1)
                self.docfilename = self.docname.lower()
                # We don't parse the Synopsis but insert the function name at the beginning
                syn = self.titles["SYNOPSIS"]
                syn = "  " + self.rettype + self.docname + "(\n" + syn
                self.titles["SYNOPSIS"] = syn
            else:
                print content
                raise ValueError("No field 'NAME' in autodoc")
                
        # append pair of brackets
        self.docname += "()"
        self.docfilename = self.docname.lower()


    def write(self, filehandle, titles):
        """Write autodoc to file.
        
        Arguments:
        
        filehandle -    filehandle of a file to write the autodoc in
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.
        """

        filehandle.write(self.docname + "\n")
        filehandle.write("=" * len(self.docname) + "\n\n")
        autodoc.write(self, filehandle, titles)
        filehandle.write("\n")


class hiddautodoc(autodoc):
    """Autodoc class for HIDD classes.
    """
    
    def __init__(self, content):
        """Constructor. Reads and parses autodoc string.

        Arguments:

        content - String of autodoc text chunk without sourrounding comment lines
        """

        autodoc.__init__(self, content)
        self.prevdocfilename = ""
        self.nextdocfilename = ""

        # get docname
        self.docname = self.titles["NAME"].split()[0]
        if self.docname == "":
            raise ValueError("docname is empty")
        self.docfilename = self.docname.lower()

    def write(self, filehandle, titles):
        """Write autodoc elements to file.
        
        Arguments:
        
        filehandle -    filehandle of a file to write the autodoc in
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.

        """

        filehandle.write(self.docname + "\n")
        filehandle.write("=" * len(self.docname) + "\n\n")
        autodoc.write(self, filehandle, titles)
        filehandle.write("\n")


class shelldoclist:
    """List of Shell autodocs
    
    Handles the Shell autodocs of a single directory.
    """

    def __init__(self):
        """Constructor.
        """

        self.doclist = []
    
    def read(self, srcdir):
        """Scan directory for autodocs
        
        Reads all *.c files of the given directory and scans them for autodocs.
        
        Arguments:
        
        srcdir - directory from which the autodocs should be read
        """
        
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

    def write_index(self, filehandle, targetdir):
        """Write index file.
        
        Arguments:
        
        filehandle - handle of a file where the index (TOC) should be written
        targetdir - directory which should be listed
        """
        
        print "Creating index file"
        filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")
        filehandle.write("==============\n")
        filehandle.write("Shell Commands\n")
        filehandle.write("==============\n\n")
        filehandle.write("+ `Introduction <introduction>`_\n\n")

        filehandle.write("+ Commands\n\n")

        write_index(filehandle, targetdir, 4)

        filehandle.write("\n+ `Scripts <scripts>`_\n")
    
    def write(self, targetdir, titles):
        """Write autodocs to directory.
        
        Each autodoc will be written in a single file.
        
        Arguments:
        
        targedir -      name of a directory to where autodocs and index should be written
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.
        """
        
        for doc in self.doclist:
            filename = os.path.join(targetdir, doc.docfilename + ".en")
            print "Writing to file", filename
            filehandle = open(filename, "w")
            doc.write(filehandle, titles)
            doc.write_xref(filehandle, ".", "../../developers/autodocs", "../../developers/headerfiles")
            filehandle.close()
        
        # create index page
        filehandle = open(os.path.join(targetdir, "index.en"), "w")
        self.write_index(filehandle, targetdir)
        filehandle.close()


class libdoclist:
    """List of autodocs of static and shared libraries.
    
    Handles the autodocs of a single directory.
    """

    def __init__(self):
        """Constructor.
        """
        
        self.doclist = []
        self.docfilename = ""

    def read(self, srcdir, name=None):
        """Scan directory for autodocs.
        
        Reads all *.c files of the given directory and scans them for autodocs.
        
        Arguments:
        
        srcdir - directory from which the autodocs should be read
        name -  Name of the document. If no name is given the rightmost part of
                srcdir will be used. This name will be later used for storing the file.
        """

        if name:
            self.docfilename = name
        else:
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
        """Write autodocs to directory.
        
        All autodocs will be written in a single file.
        
        Arguments:
        
        targedir -      name of a directory to where autodocs and index should be written
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.
        """

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
                doc.write_xref(filehandle, "../../users/shell", ".", "../headerfiles")
                # write transition
                if doc is not self.doclist[-1]:
                    filehandle.write("----------\n\n")
            filehandle.close()


class appsdoclist(shelldoclist):
    """List of autodocs of applications.
    
    Handles the autodocs of a single directory.
    """

    def write_index(self, filehandle, targetdir):
        """Write index file.
        
        Arguments:
        
        filehandle - handle of a file where the index (TOC) should be written.
        targetdir - directory which should be listed
        """
        
        # create index page
        filehandle = open(os.path.join(targetdir, "index.en"), "w")
        print "Creating index file"
        filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")
        filehandle.write("==============\n")
        filehandle.write("Applications\n")
        filehandle.write("==============\n\n")
        write_index(filehandle, targetdir, 0)
        filehandle.close()


class hidddoclist:
    """List of autodocs of HIDD classes.
    
    Handles the autodocs of a single directory.
    """

    def __init__(self):
        """Constructor.
        """
        
        self.doclist = {} # key is classname, content is list of documents per class
        self.docfilename = ""

    def read(self, srcdir, name=None):
        """Scan directory for autodocs.
        
        Reads all *.c files of the given directory and scans them for autodocs.
        
        Arguments:
        
        srcdir - directory from which the autodocs should be read
        name -  Name of the document. If no name is given the rightmost part of
                srcdir will be used. This name will be later used for storing the file.
        """

        if name:
            self.docfilename = name
        else:
            self.docfilename = os.path.basename(srcdir) + "_hidd" # rightmost part of the path
        
        cfiles = os.listdir(srcdir)
        for file in cfiles:
            if not (file in blacklist) and file[-2:]==".c":
                filename = os.path.join(srcdir, file)
                print "Reading from file", filename
                filehandle = open(filename)
                content = filehandle.read() # read whole file
                for ad_entry in ad_regx.findall(content):
                    ad = hiddautodoc(ad_entry)
                    if ad.docname != "":
                        if ad.titles.has_key("LOCATION"):
                            classname = ad.titles["LOCATION"].strip()
                            if self.doclist.has_key(classname):
                                self.doclist[classname].append(ad)
                            else:
                                self.doclist[classname] = [ad]    
                        else:
                            raise ValueError("hidd doc has no LOCATION")
                        
                filehandle.close()

        for k,v in self.doclist.iteritems():
            v.sort()

    def write(self, targetdir, titles):
        """Write autodocs to directory.
        
        All autodocs will be written in a single file.
        
        Arguments:
        
        targedir -      name of a directory to where autodocs and index should be written
        titles -        what titles (e.g. Synopsis, Note) should be printed.
                        Write them in the given capitalization.
        """

        if len(self.doclist) > 0:
            filename = os.path.join(targetdir, self.docfilename + ".en")
            print "Writing to file", filename
            filehandle = open(filename, "w")

            # create header
            underline = "=" * len(self.docfilename)
            filehandle.write(underline + "\n")
            filehandle.write(self.docfilename + "\n")
            filehandle.write(underline + "\n\n")

            filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")
            filehandle.write("`Index <index>`_\n\n")
            filehandle.write("----------\n\n")

            # create list of classes
            if len(self.doclist) > 1: # only when more than one class exists
                filehandle.write("Classes\n-------\n\n")
                for classname, doclist in self.doclist.iteritems():
                    filehandle.write("+ `" + classname + "`_\n")
                filehandle.write("\n----------\n\n")

            for classname, doclist in self.doclist.iteritems():
                if len(self.doclist) > 1:
                    filehandle.write(classname + "\n" + len(classname) * "-" + "\n\n")
 
                # create toc
                tablesep = (("=" * 42) + " ") * 4
                filehandle.write(tablesep + "\n")
                for docnr in range(len(doclist)):
                    tocname = "`" + doclist[docnr].docname + "`_"
                    tocname = tocname.ljust(43)
                    filehandle.write(tocname)
                    if (docnr + 1) % 4 == 0:
                        filehandle.write("\n")
                filehandle.write("\n" + tablesep)
                filehandle.write("\n\n-----------\n\n")

                for doc in doclist:
                    doc.write(filehandle, titles)
                    doc.write_xref(filehandle, "../../users/shell", ".", "../headerfiles")
                    # write transition
                    if doc is not doclist[-1]: # not last entry
                        filehandle.write("----------\n\n")
            filehandle.close()


def write_index(filehandle, targetdir, indent):
    """Append directory listing to index file
    
    Arguments:
    
    filehandle - file where directory listing should be appended
    targedir - directory which should be listed
    indent - number of spaces before output
    """
        
    files = os.listdir(targetdir)
    files.sort()

    for doc in files:
        if doc[-3:] == ".en" and doc[:5] != "index" and doc != ".svn" and doc[:12] != "introduction":
            docname = doc[:-3]
            filehandle.write("%s+ `%s <%s>`_\n" %(indent * " ", docname, docname))
    
def create_module_docs():
    """Create the module docs.
    """
    
    module_titles = ("Synopsis","Template","Function",
        "Inputs","Tags", "Result","Example","Notes","Bugs","See also")  # The titles we want
                                                                        # to be printed
    targetdir = os.path.join("documentation", "developers", "autodocs")
    srcdirs = ( os.path.join(topdir, "rom"), os.path.join(topdir, "rom/devs"), os.path.join(topdir, "workbench/libs") )
    for dir in srcdirs:
        create_lib_docs_dir(dir, targetdir, module_titles)

    # add some docs for linker libs in AROS/compiler
    subdirs = ( os.path.join(topdir, "compiler", "alib"),
                os.path.join(topdir, "compiler", "arossupport"),
                os.path.join(topdir, "compiler", "clib") )
    
    for docpath in subdirs:
        libdocs = libdoclist()
        libdocs.read(docpath)
        libdocs.write(targetdir, module_titles)

    # add HIDD docs
    srcdirs = ( os.path.join(topdir, "rom/hidds"), os.path.join(topdir, "workbench/hidds/"))
    for dir in srcdirs:
        create_hidd_docs_dir(dir, targetdir, module_titles)

    # print index file
    filehandle = open(os.path.join(targetdir, "index.en"), "w")
    print "Creating index.en"
    filehandle.write("======================\n")
    filehandle.write("Autodocs for Modules\n")
    filehandle.write("======================\n\n")
    filehandle.write(".. This document is automatically generated. Don't edit it!\n\n")

    write_index(filehandle, targetdir, 0)
    filehandle.close()
    print "Done"

def create_lib_docs_dir(srcdir, targetdir, titles):
    """Scan whole parent directory.
    
    Scans a parent directory like AROS/rom for subdirectories with
    *.c files with autodoc headers.
    
    Arguments:
    
    srcdir -    parent directory of directory with autodocs.
    targetdir - the directory where the resulting ReST files should be stored
    titles -    what titles (e.g. Synopsis, Note) should be printed.
                Write them in the given capitalization.
    """
    
    subdirs = os.listdir(srcdir)
    for dir in subdirs: # exec ,graphics etc.
        docpath = os.path.join(srcdir, dir)
        if (dir != ".svn") and os.path.isdir(docpath) :
            libdocs = libdoclist()
            libdocs.read(docpath)
            libdocs.write(targetdir, titles)

def create_hidd_docs_dir(srcdir, targetdir, titles):
    """Scan whole parent directory.
    
    Scans a parent directory like AROS/rom/hidds for subdirectories with
    *.c files with autodoc headers.
    
    Arguments:
    
    srcdir -    parent directory of directory with autodocs.
    targetdir - the directory where the resulting ReST files should be stored
    titles -    what titles (e.g. Synopsis, Note) should be printed.
                Write them in the given capitalization.
    """
    
    subdirs = os.listdir(srcdir)
    for dir in subdirs: # kbd, mouse, graphics etc.
        docpath = os.path.join(srcdir, dir)
        if (dir != ".svn") and os.path.isdir(docpath) :
            hidddocs = hidddoclist()
            hidddocs.read(docpath)
            hidddocs.write(targetdir, titles)

def create_shell_docs():
    """Create the Shell commands docs.
    """
    
    srcdirs = ( os.path.join(topdir, "workbench", "c"),
                os.path.join(topdir, "workbench", "c", "shellcommands"),
                os.path.join(topdir, "workbench", "c", "Identify"),
                os.path.join(topdir, "workbench", "c", "Partition"),
                os.path.join(topdir, "workbench", "c", "Decoration"),
                os.path.join(topdir, "workbench", "c", "iprefs") )
                
    targetdir = os.path.join("documentation", "users", "shell") # relative to main build script
    shell_titles = ("Name","Format","Template","Synopsis","Location","Function",
        "Inputs","Tags","Result","Example","Notes","Bugs","See also")  # The titles we want
                                                                       # to be printed
    shelldocs = shelldoclist()
    for dir in srcdirs:
        shelldocs.read(dir)
    shelldocs.write(targetdir, shell_titles)
    print "Done"

def create_apps_docs():
    """Create the application docs.
    """
    
    srcdirs = ( os.path.join(topdir, "workbench", "tools", "commodities"),
                os.path.join(topdir, "workbench", "tools") )
                
    targetdir = os.path.join("documentation", "users", "applications") # relative to main build script
    apps_titles = ("Name","Format","Template","Synopsis","Location","Function",
        "Inputs","Tags","Result","Example","Notes","Bugs","See also")  # The titles we want
                                                                       # to be printed
    appsdocs = appsdoclist()
    for dir in srcdirs:
        appsdocs.read(dir)
    appsdocs.write(targetdir, apps_titles)
    print "Done"

def create_all_docs():
    """Create all docs.
    """
    create_shell_docs()
    create_module_docs()
    create_apps_docs()


if __name__ == "__main__":
    os.chdir("..") # because we have relative paths
    create_all_docs()
