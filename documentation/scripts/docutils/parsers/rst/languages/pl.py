# Author: Tomasz Paul
# Contact: grubert@users.sourceforge.net
# Revision: $Revision: 24659 $
# Date: $Date: 2006-08-27 19:45:24 +0200 (Śr, 27 sie 2006) $
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
      'uwaga': 'attention',
      'ostrzeżenie': 'caution',
      'niebezpieczeństwo': 'danger',
      'błąd': 'error',
      'wzmianka': 'hint',
      'ważne': 'important',
      'przypis': 'note',
      'wskazówka': 'tip',
      'ostrzeżenie': 'warning',
      'przestroga': 'admonition',
      'sidebar (translation required)': 'sidebar',
      'sidebar (translation required)': 'sidebar',
      'temat': 'topic',
      'line-block (translation required)': 'line-block',
      'parsed-literal (translation required)': 'parsed-literal',
      'rubryka': 'rubric',
      'epigraph (translation required)': 'epigraph',
      'highlights (translation required)': 'highlights',
      'pull-quote (translation required)': 'pull-quote', 
      'zusammengesetzt (translation required)': 'compound',
      'verbund (translation required)': 'compound',
      u'container (translation required)': 'container',
      #'pytania': 'questions',
      'tabela': 'table',
      'tabela csv': 'csv-table',
      'list-table (translation required)': 'list-table',
      'meta': 'meta',
      #'imagemap (translation required)': 'imagemap',
      'obraz': 'image',
      'figure (translation required)': 'figure',
      u'raw (translation required)': 'raw',
      u'raw (translation required)': 'raw',
      'dołącz': 'include',
      'zamiana': 'replace',
      'zamienić': 'replace',
      'zamień': 'replace',
      'unicode': 'unicode',
      'data': 'date',
      'klasa': 'class',
      'role (translation required)': 'role',
      u'default-role (translation required)': 'default-role',
      u'title (translation required)': 'title',
      'zawartość': 'contents',
      'numeracja sekcji': 'sectnum',
      'sectnum (translation required)': 'sectnum',
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
      'akronim': 'acronym',
      'index': 'index',
      'indeks dolny': 'subscript',
      'indeks górny': 'superscript',
      'title-reference': 'title-reference',
      'pep-reference (translation required)': 'pep-reference',
      'rfc-reference (translation required)': 'rfc-reference',
      'emphasis (translation required)': 'emphasis',
      'pogrubienie': 'strong',
      u'literal (translation required)': 'literal',
      'named-reference (translation required)': 'named-reference',
      'anonimowa referencja': 'anonymous-reference',
      u'footnote-reference (translation required)': 'footnote-reference',
      'citation-reference (translation required)': 'citation-reference',
      'substitution-reference (translation required)': 'substitution-reference',
      'cel': 'target',
      'uri-reference (translation required)': 'uri-reference',
      u'raw (translation required)': 'raw',
      u'raw (translation required)': 'raw',}
"""Mapping of Polish role names to canonical role names for interpreted text.
"""
