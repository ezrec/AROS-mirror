# Author: Ioannis Koniaris
# Contact: ikoniari@csd.auth.gr
# Revision: $Revision: 24659 $
# Date: $Date: 2006-08-23 02:45:24 +0300 (Τετ, 23 Αυγ 2006) $
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

# XREIAZETAI ANATHEORISI 

directives = {
      'προσοχή': 'attention',
      'προσοχή': 'caution',
      'κίνδυνος': 'danger',
      'σφάλμα': 'error',
      'σημείωση': 'hint',
      'σημαντικό': 'important',
      'σημείωση': 'note',
      'συμβουλή': 'tip',
      'προειδοποίηση': 'warning',
      'προειδοποίηση': 'admonition',
      'πλαϊνή μπάρα': 'sidebar',
      'θέμα': 'topic',
      'γραμμή-φραγμός': 'line-block',
      'parsed-literal': 'parsed-literal',
      'ρουμπρίκα': 'rubric',
      'επιγραφή': 'epigraph',
      'κυριώτερα σημεία': 'highlights',
      'pull-quote': 'pull-quote',
      'ένωση': 'compound',
      'κιβώτιο': 'container',
      #'ερωτήσεις': 'questions',
      'πίνακας': 'table',
      'πίνακας-csv': 'csv-table',
      'list-table': 'list-table',
      #'faq': 'questions',
      'meta': 'meta',
      #'imagemap': 'imagemap',
      'εικόνα': 'image',
      'απεικόνιση': 'figure',
      'συμπεριλαμβάνω': 'include',
      'raw': 'raw',
      'αντικαθιστώ': 'replace',
      'unicode': 'unicode',
      'ημερομηνία': 'date',
      'κατηγορία': 'class',
      'ρόλος': 'role',
      'ρόλος-προεπιλογή': 'default-role',
      'τίτλος': 'title',
      'περιεχόμενα': 'contents',
      'αρίθμηση-τμήματος': 'sectnum',
      'επικεφαλίδα': 'header',
      'υποσημείωση': 'footer',
      #'υποσημειώσεις': 'footnotes',
      #'παραπομπές': 'citations',
      'target-notes': 'target-notes',
      'restructuredtext-test-directive': 'restructuredtext-test-directive'}
"""Greek name to registered (in directives/__init__.py) directive name
mapping."""

roles = {
    'σύντμηση': 'abbreviation',
    'αρκτικόλεξο': 'acronym',
    'index': 'index',
    'subscript': 'subscript',
    'superscript': 'superscript',
    'αναφορά-τίτλου': 'title-reference',
    'τίτλος': 'title-reference',
    'pep-reference': 'pep-reference',
    'pep': 'pep-reference',
    'rfc-reference': 'rfc-reference',
    'rfc': 'rfc-reference',
    'έμφαση': 'emphasis',
    'δυνατός': 'strong',
    'κυριολεκτικός': 'literal',
    'επώνυμη-αναφορά': 'named-reference',
    'ανώνυμη-αναφορά': 'anonymous-reference',
    'αναφορά-υποσυμείωσης': 'footnote-reference',
    'αναφορά-παραπομπής': 'citation-reference',
    'αναφορά-αντικατάστασης': 'substitution-reference',
    'στόχος': 'target',
    'uri-reference': 'uri-reference',
    'uri': 'uri-reference',
    'url': 'uri-reference',
    'raw': 'raw',}
"""Mapping of Greek role names to canonical role names for interpreted text.
"""
