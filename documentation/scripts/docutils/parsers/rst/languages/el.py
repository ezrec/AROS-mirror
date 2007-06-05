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
Greek-language mappings for language-dependent features of
reStructuredText.
"""

__docformat__ = 'reStructuredText'

directives = {
      u'\u03c0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae': u'attention',
      u'\u03c0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae': u'caution',
      u'\u03ba\u03af\u03bd\u03b4\u03c5\u03bd\u03bf\u03c2': u'danger',
      u'\u03c3\u03c6\u03ac\u03bb\u03bc\u03b1': u'error',
      u'\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': u'hint',
      u'\u03c3\u03b7\u03bc\u03b1\u03bd\u03c4\u03b9\u03ba\u03cc': u'important',
      u'\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': u'note',
      u'\u03c3\u03c5\u03bc\u03b2\u03bf\u03c5\u03bb\u03ae': u'tip',
      u'\u03c0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7': u'warning',
      u'\u03c0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7': u'admonition',
      u'\u03c0\u03bb\u03b1\u03ca\u03bd\u03ae \u03bc\u03c0\u03ac\u03c1\u03b1': u'sidebar',
      u'\u03b8\u03ad\u03bc\u03b1': u'topic',
      u'\u03b3\u03c1\u03b1\u03bc\u03bc\u03ae-\u03c6\u03c1\u03b1\u03b3\u03bc\u03cc\u03c2': u'line-block',
      u'parsed-literal': u'parsed-literal',
      u'\u03c1\u03bf\u03c5\u03bc\u03c0\u03c1\u03af\u03ba\u03b1': u'rubric',
      u'\u03b5\u03c0\u03b9\u03b3\u03c1\u03b1\u03c6\u03ae': u'epigraph',
      u'\u03ba\u03c5\u03c1\u03b9\u03ce\u03c4\u03b5\u03c1\u03b1 \u03c3\u03b7\u03bc\u03b5\u03af\u03b1': u'highlights',
      u'pull-quote': u'pull-quote',
      u'\u03ad\u03bd\u03c9\u03c3\u03b7': u'compound',
      u'\u03ba\u03b9\u03b2\u03ce\u03c4\u03b9\u03bf': u'container',
      #'\u03b5\u03c1\u03c9\u03c4\u03ae\u03c3\u03b5\u03b9\u03c2': 'questions',
      u'\u03c0\u03af\u03bd\u03b1\u03ba\u03b1\u03c2': u'table',
      u'\u03c0\u03af\u03bd\u03b1\u03ba\u03b1\u03c2-csv': u'csv-table',
      u'list-table': 'list-table',
      #'faq': 'questions',
      u'meta': u'meta',
      #'imagemap': 'imagemap',
      u'\u03b5\u03b9\u03ba\u03cc\u03bd\u03b1': u'image',
      u'\u03b1\u03c0\u03b5\u03b9\u03ba\u03cc\u03bd\u03b9\u03c3\u03b7': u'figure',
      u'\u03c3\u03c5\u03bc\u03c0\u03b5\u03c1\u03b9\u03bb\u03b1\u03bc\u03b2\u03ac\u03bd\u03c9': u'include',
      u'raw': u'raw',
      u'\u03b1\u03bd\u03c4\u03b9\u03ba\u03b1\u03b8\u03b9\u03c3\u03c4\u03ce': u'replace',
      u'unicode': u'unicode',
      u'\u03b7\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1': u'date',
      u'\u03ba\u03b1\u03c4\u03b7\u03b3\u03bf\u03c1\u03af\u03b1': u'class',
      u'\u03c1\u03cc\u03bb\u03bf\u03c2': u'role',
      u'\u03c1\u03cc\u03bb\u03bf\u03c2-\u03c0\u03c1\u03bf\u03b5\u03c0\u03b9\u03bb\u03bf\u03b3\u03ae': u'default-role',
      u'\u03c4\u03af\u03c4\u03bb\u03bf\u03c2': u'title',
      u'\u03c0\u03b5\u03c1\u03b9\u03b5\u03c7\u03cc\u03bc\u03b5\u03bd\u03b1': u'contents',
      u'\u03b1\u03c1\u03af\u03b8\u03bc\u03b7\u03c3\u03b7-\u03c4\u03bc\u03ae\u03bc\u03b1\u03c4\u03bf\u03c2': u'sectnum',
      u'\u03b5\u03c0\u03b9\u03ba\u03b5\u03c6\u03b1\u03bb\u03af\u03b4\u03b1': u'header',
      u'\u03c5\u03c0\u03bf\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': u'footer',
      #'\u03c5\u03c0\u03bf\u03c3\u03b7\u03bc\u03b5\u03b9\u03ce\u03c3\u03b5\u03b9\u03c2': 'footnotes',
      #'\u03c0\u03b1\u03c1\u03b1\u03c0\u03bf\u03bc\u03c0\u03ad\u03c2': 'citations',
      u'target-notes': u'target-notes',
      u'restructuredtext-test-directive': u'restructuredtext-test-directive'}
"""Greek name to registered (in directives/__init__.py) directive name
mapping."""

roles = {
    u'\u03c3\u03cd\u03bd\u03c4\u03bc\u03b7\u03c3\u03b7': u'abbreviation',
    u'\u03b1\u03c1\u03ba\u03c4\u03b9\u03ba\u03cc\u03bb\u03b5\u03be\u03bf': u'acronym',
    u'index': u'index',
    u'subscript': u'subscript',
    u'superscript': u'superscript',
    u'\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c4\u03af\u03c4\u03bb\u03bf\u03c5': u'title-reference',
    u'\u03c4\u03af\u03c4\u03bb\u03bf\u03c2': u'title-reference',
    u'pep-reference': u'pep-reference',
    u'pep': u'pep-reference',
    u'rfc-reference': u'rfc-reference',
    u'rfc': u'rfc-reference',
    u'\u03ad\u03bc\u03c6\u03b1\u03c3\u03b7': u'emphasis',
    u'\u03b4\u03c5\u03bd\u03b1\u03c4\u03cc\u03c2': u'strong',
    u'\u03ba\u03c5\u03c1\u03b9\u03bf\u03bb\u03b5\u03ba\u03c4\u03b9\u03ba\u03cc\u03c2': u'literal',
    u'\u03b5\u03c0\u03ce\u03bd\u03c5\u03bc\u03b7-\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac': u'named-reference',
    u'\u03b1\u03bd\u03ce\u03bd\u03c5\u03bc\u03b7-\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac': u'anonymous-reference',
    u'\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c5\u03c0\u03bf\u03c3\u03c5\u03bc\u03b5\u03af\u03c9\u03c3\u03b7\u03c2': u'footnote-reference',
    u'\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c0\u03b1\u03c1\u03b1\u03c0\u03bf\u03bc\u03c0\u03ae\u03c2': u'citation-reference',
    u'\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03b1\u03bd\u03c4\u03b9\u03ba\u03b1\u03c4\u03ac\u03c3\u03c4\u03b1\u03c3\u03b7\u03c2': u'substitution-reference',
    u'\u03c3\u03c4\u03cc\u03c7\u03bf\u03c2': u'target',
    u'uri-reference': u'uri-reference',
    u'uri': u'uri-reference',
    u'url': u'uri-reference',
    u'raw': u'raw',}
"""Mapping of Greek role names to canonical role names for interpreted text.
"""
