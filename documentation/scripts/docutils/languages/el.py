# Author: Ioannis Koniaris
# Contact: ikoniaris@gmail.com
# Revision: $Revision$
# Date: $Date$
# Copyright: This module has been placed in the public domain.

# New language mappings are welcome.  Before doing a new translation, please
# read <http://docutils.sf.net/docs/howto/i18n.html>.  Two files must be
# translated for each language: one in docutils/languages, the other in
# docutils/parsers/rst/languages.

"""
Greek-language mappings for language-dependent features of Docutils.
"""

__docformat__ = 'reStructuredText'

labels = {
      u'author': u'\u03a3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03ad\u03b1\u03c2',
      u'authors': u'\u03a3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03b5\u03af\u03c2',
      u'organization': u'\u039f\u03c1\u03b3\u03b1\u03bd\u03b9\u03c3\u03bc\u03cc\u03c2',
      u'address': u'\u0394\u03b9\u03b5\u03cd\u03b8\u03c5\u03bd\u03c3\u03b7',
      u'contact': u'\u0395\u03c0\u03b9\u03ba\u03bf\u03b9\u03bd\u03c9\u03bd\u03af\u03b1',
      u'version': u'\u0388\u03ba\u03b4\u03bf\u03c3\u03b7',
      u'revision': u'\u0391\u03bd\u03b1\u03b8\u03b5\u03ce\u03c1\u03b7\u03c3\u03b7',
      u'status': u'\u03a3\u03c4\u03ac\u03c4\u03bf\u03c5\u03c2',
      u'date': u'\u0397\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1',
      u'copyright': u'Copyright',
      u'dedication': u'\u0391\u03c6\u03b9\u03ad\u03c1\u03c9\u03c3\u03b7',
      u'abstract': u'\u03a0\u03b5\u03c1\u03af\u03bb\u03b7\u03c8\u03b7',
      u'attention': u'\u03a0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae!',
      u'caution': u'\u03a0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae!',
      u'danger': u'!\u039a\u0399\u039d\u0394\u03a5\u039d\u039f\u03a3!',
      u'error': u'\u03a3\u03c6\u03ac\u03bb\u03bc\u03b1',
      u'hint': u'\u03a3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7',
      u'important': u'\u03a3\u03c5\u03bc\u03b1\u03bd\u03c4\u03b9\u03ba\u03cc',
      u'note': u'\u03a3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7',
      u'tip': u'\u03a3\u03c5\u03bc\u03b2\u03bf\u03c5\u03bb\u03ae',
      u'warning': u'\u03a0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7',
      u'contents': u'\u03a0\u03b5\u03c1\u03b9\u03b5\u03c7\u03cc\u03bc\u03b5\u03bd\u03b1'}
"""Mapping of node class name to label text."""

bibliographic_fields = {
      u'\u03c3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03ad\u03b1\u03c2': u'author',
      u'\u03c3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03b5\u03af\u03c2': u'authors',
      u'\u03bf\u03c1\u03b3\u03b1\u03bd\u03b9\u03c3\u03bc\u03cc\u03c2': u'organization',
      u'\u03b4\u03b9\u03b5\u03cd\u03b8\u03c5\u03bd\u03c3\u03b7': u'address',
      u'\u03b5\u03c0\u03b9\u03ba\u03bf\u03b9\u03bd\u03c9\u03bd\u03af\u03b1': u'contact',
      u'\u03ad\u03ba\u03b4\u03bf\u03c3\u03b7': u'version',
      u'\u03b1\u03bd\u03b1\u03b8\u03b5\u03ce\u03c1\u03b7\u03c3\u03b7': u'revision',
      u'\u03c3\u03c4\u03ac\u03c4\u03bf\u03c5\u03c2': u'status',
      u'\u03b7\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1': u'date',
      u'copyright': u'copyright',
      u'\u03b1\u03c6\u03b9\u03ad\u03c1\u03c9\u03c3\u03b7': u'dedication',
      u'\u03c0\u03b5\u03c1\u03af\u03bb\u03b7\u03c8\u03b7': u'abstract'}
"""Greek (lowcased) to canonical name mapping for bibliographic fields."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
