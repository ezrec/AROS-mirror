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
      '\u03c0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae': 'attention',
      '\u03c0\u03c1\u03bf\u03c3\u03bf\u03c7\u03ae': 'caution',
      '\u03ba\u03af\u03bd\u03b4\u03c5\u03bd\u03bf\u03c2': 'danger',
      '\u03c3\u03c6\u03ac\u03bb\u03bc\u03b1': 'error',
      '\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': 'hint',
      '\u03c3\u03b7\u03bc\u03b1\u03bd\u03c4\u03b9\u03ba\u03cc': 'important',
      '\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': 'note',
      '\u03c3\u03c5\u03bc\u03b2\u03bf\u03c5\u03bb\u03ae': 'tip',
      '\u03c0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7': 'warning',
      '\u03c0\u03c1\u03bf\u03b5\u03b9\u03b4\u03bf\u03c0\u03bf\u03af\u03b7\u03c3\u03b7': 'admonition',
      '\u03c0\u03bb\u03b1\u03ca\u03bd\u03ae \u03bc\u03c0\u03ac\u03c1\u03b1': 'sidebar',
      '\u03b8\u03ad\u03bc\u03b1': 'topic',
      '\u03b3\u03c1\u03b1\u03bc\u03bc\u03ae-\u03c6\u03c1\u03b1\u03b3\u03bc\u03cc\u03c2': 'line-block',
      'parsed-literal': 'parsed-literal',
      '\u03c1\u03bf\u03c5\u03bc\u03c0\u03c1\u03af\u03ba\u03b1': 'rubric',
      '\u03b5\u03c0\u03b9\u03b3\u03c1\u03b1\u03c6\u03ae': 'epigraph',
      '\u03ba\u03c5\u03c1\u03b9\u03ce\u03c4\u03b5\u03c1\u03b1 \u03c3\u03b7\u03bc\u03b5\u03af\u03b1': 'highlights',
      'pull-quote': 'pull-quote',
      '\u03ad\u03bd\u03c9\u03c3\u03b7': 'compound',
      '\u03ba\u03b9\u03b2\u03ce\u03c4\u03b9\u03bf': 'container',
      #'\u03b5\u03c1\u03c9\u03c4\u03ae\u03c3\u03b5\u03b9\u03c2': 'questions',
      '\u03c0\u03af\u03bd\u03b1\u03ba\u03b1\u03c2': 'table',
      '\u03c0\u03af\u03bd\u03b1\u03ba\u03b1\u03c2-csv': 'csv-table',
      'list-table': 'list-table',
      #'faq': 'questions',
      'meta': 'meta',
      #'imagemap': 'imagemap',
      '\u03b5\u03b9\u03ba\u03cc\u03bd\u03b1': 'image',
      '\u03b1\u03c0\u03b5\u03b9\u03ba\u03cc\u03bd\u03b9\u03c3\u03b7': 'figure',
      '\u03c3\u03c5\u03bc\u03c0\u03b5\u03c1\u03b9\u03bb\u03b1\u03bc\u03b2\u03ac\u03bd\u03c9': 'include',
      'raw': 'raw',
      '\u03b1\u03bd\u03c4\u03b9\u03ba\u03b1\u03b8\u03b9\u03c3\u03c4\u03ce': 'replace',
      'unicode': 'unicode',
      '\u03b7\u03bc\u03b5\u03c1\u03bf\u03bc\u03b7\u03bd\u03af\u03b1': 'date',
      '\u03ba\u03b1\u03c4\u03b7\u03b3\u03bf\u03c1\u03af\u03b1': 'class',
      '\u03c1\u03cc\u03bb\u03bf\u03c2': 'role',
      '\u03c1\u03cc\u03bb\u03bf\u03c2-\u03c0\u03c1\u03bf\u03b5\u03c0\u03b9\u03bb\u03bf\u03b3\u03ae': 'default-role',
      '\u03c4\u03af\u03c4\u03bb\u03bf\u03c2': 'title',
      '\u03c0\u03b5\u03c1\u03b9\u03b5\u03c7\u03cc\u03bc\u03b5\u03bd\u03b1': 'contents',
      '\u03b1\u03c1\u03af\u03b8\u03bc\u03b7\u03c3\u03b7-\u03c4\u03bc\u03ae\u03bc\u03b1\u03c4\u03bf\u03c2': 'sectnum',
      '\u03b5\u03c0\u03b9\u03ba\u03b5\u03c6\u03b1\u03bb\u03af\u03b4\u03b1': 'header',
      '\u03c5\u03c0\u03bf\u03c3\u03b7\u03bc\u03b5\u03af\u03c9\u03c3\u03b7': 'footer',
      #'\u03c5\u03c0\u03bf\u03c3\u03b7\u03bc\u03b5\u03b9\u03ce\u03c3\u03b5\u03b9\u03c2': 'footnotes',
      #'\u03c0\u03b1\u03c1\u03b1\u03c0\u03bf\u03bc\u03c0\u03ad\u03c2': 'citations',
      'target-notes': 'target-notes',
      'restructuredtext-test-directive': 'restructuredtext-test-directive'}
"""Greek name to registered (in directives/__init__.py) directive name
mapping."""

roles = {
    '\u03c3\u03cd\u03bd\u03c4\u03bc\u03b7\u03c3\u03b7': 'abbreviation',
    '\u03b1\u03c1\u03ba\u03c4\u03b9\u03ba\u03cc\u03bb\u03b5\u03be\u03bf': 'acronym',
    'index': 'index',
    'subscript': 'subscript',
    'superscript': 'superscript',
    '\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c4\u03af\u03c4\u03bb\u03bf\u03c5': 'title-reference',
    '\u03c4\u03af\u03c4\u03bb\u03bf\u03c2': 'title-reference',
    'pep-reference': 'pep-reference',
    'pep': 'pep-reference',
    'rfc-reference': 'rfc-reference',
    'rfc': 'rfc-reference',
    '\u03ad\u03bc\u03c6\u03b1\u03c3\u03b7': 'emphasis',
    '\u03b4\u03c5\u03bd\u03b1\u03c4\u03cc\u03c2': 'strong',
    '\u03ba\u03c5\u03c1\u03b9\u03bf\u03bb\u03b5\u03ba\u03c4\u03b9\u03ba\u03cc\u03c2': 'literal',
    '\u03b5\u03c0\u03ce\u03bd\u03c5\u03bc\u03b7-\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac': 'named-reference',
    '\u03b1\u03bd\u03ce\u03bd\u03c5\u03bc\u03b7-\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac': 'anonymous-reference',
    '\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c5\u03c0\u03bf\u03c3\u03c5\u03bc\u03b5\u03af\u03c9\u03c3\u03b7\u03c2': 'footnote-reference',
    '\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03c0\u03b1\u03c1\u03b1\u03c0\u03bf\u03bc\u03c0\u03ae\u03c2': 'citation-reference',
    '\u03b1\u03bd\u03b1\u03c6\u03bf\u03c1\u03ac-\u03b1\u03bd\u03c4\u03b9\u03ba\u03b1\u03c4\u03ac\u03c3\u03c4\u03b1\u03c3\u03b7\u03c2': 'substitution-reference',
    '\u03c3\u03c4\u03cc\u03c7\u03bf\u03c2': 'target',
    'uri-reference': 'uri-reference',
    'uri': 'uri-reference',
    'url': 'uri-reference',
    'raw': 'raw',}
"""Mapping of Greek role names to canonical role names for interpreted text.
"""
