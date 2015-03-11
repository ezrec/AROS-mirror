# -*- coding: iso-8859-1 -*-
# Copyright © 2015, The AROS Development Team. All rights reserved.
# $Id$

from trac.wiki.macros import WikiMacroBase
from trac.versioncontrol.api import RepositoryManager

# creates the macro [[ArosCredits]] which inserts the content of AboutAROS/db/credits
# copy this file to /home/wwwaros/www/trac/plugins on vps2

class ArosCreditsMacro(WikiMacroBase):
    """ Displays content of 'AboutAROS/db/credits'"""

    #implements(IPermissionRequestor)

    #def get_permission_actions(self):
    #    return ['BROWSER_VIEW', 'FILE_VIEW']


    def expand_macro(self, formatter, name, content, args=None):
        rm = RepositoryManager(self.env)
        reponame, repos, path = rm.get_repository_by_path("/AROS/trunk/AROS/workbench/system/AboutAROS/db/credits")
        node = repos.get_node(path, repos.youngest_rev)
        stream = node.get_content()
        content = stream.read(10000)
        content = unicode(content, "iso8859-15")
        content = content.split("\n")
        result = ""
        listopen = False
        for line in content:
            if ":" in line:
                if listopen:
                    result = result + "</ul>\n"
                result = result + "<h2> " + line + "</h2>" + "\n<ul>\n"
                listopen = True
            elif line.strip() != "":
                result = result + "<li>" + line + "</li>\n"
        if listopen:
            result = result + "</ul>\n"
        return result
