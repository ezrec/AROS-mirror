# Authors:   Tomasz Paul
# Contact:   tpaul@wp.pl
# Revision:  $Revision: 1 $
# Date:      $Date: 2006-08-27 19:45:24 +0200 (Śr, 27 sie 2006) $
# Copyright: This module has been placed in the public domain.

# New language mappings are welcome.  Before doing a new translation, please
# read <http://docutils.sf.net/docs/howto/i18n.html>.  Two files must be
# translated for each language: one in docutils/languages, the other in
# docutils/parsers/rst/languages.

"""
Polish language mappings for language-dependent features of Docutils.
"""

__docformat__ = 'reStructuredText'

labels = {
    'author': 'autor',
    'authors': 'autorzy',
    'organization': 'organizacja',
    'address': 'adres',
    'contact': 'kontakt',
    'version': 'wersja',
    'revision': 'rewizja',
    'status': 'status',
    'date': 'data',
    'dedication': 'dedykacja',
    'copyright': 'prawa autorskie',
    'abstract': 'streszczenie',
    'attention': 'uwaga!',
    'caution': 'ostrzeżenie!',
    'danger': 'niebezpieczeństwo!',
    'error': 'błąd',
    'hint': 'wzmianka',
    'important': 'ważne',
    'note': 'przypis',
    'tip': 'wskazówka',
    'warning': 'ostrzeżenie',
    'contents': 'zawartość'}
"""Mapping of node class name to label text."""

bibliographic_fields = {
    'autor': 'author',
    'autorzy': 'authors',
    'organizacja': 'organization',
    'adres': 'address',
    'kontakt': 'contact',
    'wersja': 'version',
    'rewizja': 'revision',
    'status': 'status',
    'data': 'date',
    'prawa autorskie': 'copyright',
    'dedykacja': 'dedication',
    'streszczenie': 'abstract'}
"""Polish (lowcased) to canonical name mapping for bibliographic fields."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
