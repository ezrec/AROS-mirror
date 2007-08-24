# -*- coding: iso-8859-15 -*-
#
# File:     defaults.py
#
# Date:     2005/05/24
# Author:   Guillaume ROGUEZ <yomgui1@gmail.com>
#

from os.path import __name__ as pathname

from os.path import join as pjoin

ROOT = 'Feelin:'
LOCALE = 'LOCALE:'

defenv_locale = {'language': 'Français',
                 'catpath': 'Locale', # must be relative to root path
                 'h_templates_dir': pjoin(ROOT, 'Sources', '_Locale'), # must be relative to root path
                 'sysdir': LOCALE}

DefaultEnv = {'root': ROOT,
              'locale': defenv_locale,
              'dryrun': False,
              'verbose': False,
              'quiet': False}
