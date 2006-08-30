# Author: Tomasz Paul
# Contact: tpaul@wp.pl
# Revision: $Revision$
# Date: $Date$
# Copyright: This module has been placed in the public domain.

# New language mappings are welcome.  Before doing a new translation, please
# read <http://docutils.sf.net/docs/howto/i18n.html>.  Two files must be
# translated for each language: one in docutils/languages, the other in
# docutils/parsers/rst/languages.

"""
Polish-language mappings for language-dependent features of
reStructuredText.
"""

__docformat__ = 'reStructuredText'


directives = {
      u'uwaga': 'attention',
      u'ostrze\u017cenie': 'caution',
      u'niebezpiecze\u0144stwo': 'danger',
      u'b\u0142\u0105d': 'error',
      u'wzmianka': 'hint',
      u'wa\u017cne': 'important',
      u'przypis': 'note',
      u'wskaz\u00f3wka': 'tip',
      u'ostrze\u017cenie': 'warning',
      u'przestroga': 'admonition',
      u'sidebar (translation required)': 'sidebar',
      u'sidebar (translation required)': 'sidebar',
      u'temat': 'topic',
      u'line-block (translation required)': 'line-block',
      u'parsed-literal (translation required)': 'parsed-literal',
      u'rubryka': 'rubric',
      u'epigraph (translation required)': 'epigraph',
      u'highlights (translation required)': 'highlights',
      u'pull-quote (translation required)': 'pull-quote', 
      u'zusammengesetzt (translation required)': 'compound',
      u'verbund (translation required)': 'compound',
      u'container (translation required)': 'container',
      #'pytania': 'questions',
      u'tabela': 'table',
      u'tabela csv': 'csv-table',
      u'list-table (translation required)': 'list-table',
      u'meta': 'meta',
      #'imagemap (translation required)': 'imagemap',
      u'obraz': 'image',
      u'figure (translation required)': 'figure',
      u'raw (translation required)': 'raw',
      u'raw (translation required)': 'raw',
      u'do\u0142\u0105cz': 'include',
      u'zamiana': 'replace',
      u'zamieni\u0107': 'replace',
      u'zamie\u0144': 'replace',
      u'unicode': 'unicode',
      u'data': 'date',
      u'klasa': 'class',
      u'role (translation required)': 'role',
      u'default-role (translation required)': 'default-role',
      u'title (translation required)': 'title',
      u'spis tre\u015bci': 'contents',
      u'numeracja sekcji': 'sectnum',
      u'sectnum (translation required)': 'sectnum',
      u'target-notes (translation required)': 'target-notes',
      u'header (translation required)': 'header',
      u'footer (translation required)': 'footer',
      #u'footnotes': 'footnotes',
      #'cytat': 'citations',
      }
"""Polish name to registered (in directives/__init__.py) directive name
mapping."""

roles = {
      u'abbreviation (translation required)': 'abbreviation',
      u'akronim': 'acronym',
      u'index': 'index',
      u'indeks dolny': 'subscript',
      u'indeks g\u00f32rny': 'superscript',
      u'title-reference': 'title-reference',
      u'pep-reference (translation required)': 'pep-reference',
      u'rfc-reference (translation required)': 'rfc-reference',
      u'emphasis (translation required)': 'emphasis',
      u'pogrubienie': 'strong',
      u'literal (translation required)': 'literal',
      u'named-reference (translation required)': 'named-reference',
      u'anonimowa referencja': 'anonymous-reference',
      u'footnote-reference (translation required)': 'footnote-reference',
      u'citation-reference (translation required)': 'citation-reference',
      u'substitution-reference (translation required)': 'substitution-reference',
      u'cel': 'target',
      u'uri-reference (translation required)': 'uri-reference',
      u'raw (translation required)': 'raw',
      u'raw (translation required)': 'raw',}
"""Mapping of Polish role names to canonical role names for interpreted text.
"""
