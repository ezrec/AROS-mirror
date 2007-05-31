# Author: Ioannis Koniaris
# Contact: ikoniari@csd.auth.gr
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
      'author': '\u03a3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03ad\u03b1\u03c2',
      'authors': '\u03a3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03b5\u03af\u03c2',
      'organization': '\u039f\u03c1\u03b3\u03b1\u03bd\u03b9\u03c3\u03bc\u03cc\u03c2',
      'address': '\u0394\u03b9\u03b5\u03cd\u03b8\u03c5\u03bd\u03c3\u03b7',
      'contact': '\u0395\u03c0\u03b9\u03ba\u03bf\u03b9\u03bd\u03c9\u03bd\u03af\u03b1',
      'version': '\u0388\u03ba\u03b4\u03bf\u03c3\u03b7',
      'revision': '\u0391\u03bd\u03b1\u03b8\u03b5\u03ce\u03c1\u03b7\u03c3\u03b7',
      'status': '\u03a3\u03c4\u03ac\u03c4\u03bf\u03c5\u03c2',
      'date': '\u0397\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1',
      'copyright': 'Copyright',
      'dedication': '\u0391\u03c6\u03b9\u03ad\u03c1\u03c9\u03c3\u03b7',
      'abstract': '\u03a0\u03b5\u03c1\u03af\u03bb\u03b7\u03c8\u03b7',
      'attention': '\u03a0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae!',
      'caution': '\u03a0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae!',
      'danger': '!\u039a\u0399\u039d\u0394\u03a5\u039d\u039f\u03a3!',
      'error': '\u03a3\u03c6\u03ac\u03bb\u03bc\u03b1',
      'hint': '\u03a3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7',
      'important': '\u03a3\u03c5\u03bc\u03b1\u03bd\u03c4\u03b9\u03ba\u03cc',
      'note': '\u03a3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7',
      'tip': '\u03a3\u03c5\u03bc\u03b2\u03bf\u03c5\u03bb\u03ae',
      'warning': '\u03a0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7',
      'contents': '\u03a0\u03b5\u03c1\u03b9\u03b5\u03c7\u03cc\u03bc\u03b5\u03bd\u03b1'}
"""Mapping of node class name to label text."""

bibliographic_fields = { 
      '\u03c3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03ad\u03b1\u03c2': 'author',
      '\u03c3\u03c5\u03b3\u03b3\u03c1\u03b1\u03c6\u03b5\u03af\u03c2': 'authors',
      '\u03bf\u03c1\u03b3\u03b1\u03bd\u03b9\u03c3\u03bc\u03cc\u03c2': 'organization',
      '\u03b4\u03b9\u03b5\u03cd\u03b8\u03c5\u03bd\u03c3\u03b7': 'address',
      '\u03b5\u03c0\u03b9\u03ba\u03bf\u03b9\u03bd\u03c9\u03bd\u03af\u03b1': 'contact',
      '\u03ad\u03ba\u03b4\u03bf\u03c3\u03b7': 'version',
      '\u03b1\u03bd\u03b1\u03b8\u03b5\u03ce\u03c1\u03b7\u03c3\u03b7': 'revision',
      '\u03c3\u03c4\u03ac\u03c4\u03bf\u03c5\u03c2': 'status',
      '\u03b7\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1': 'date',
      'copyright': 'copyright',
      '\u03b1\u03c6\u03b9\u03ad\u03c1\u03c9\u03c3\u03b7': 'dedication',
      '\u03c0\u03b5\u03c1\u03af\u03bb\u03b7\u03c8\u03b7': 'abstract'}
"""Greek (lowcased) to canonical name mapping for bibliographic fields."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
