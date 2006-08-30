# Authors:   Tomasz Paul
# Contact:   tpaul@wp.pl
# Revision:  $Revision$
# Date:      $Date$
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
    u'author': u'autor',
    u'authors': u'autorzy',
    u'organization': u'organizacja',
    u'address': u'adres',
    u'contact': u'kontakt',
    u'version': u'wersja',
    u'revision': u'rewizja',
    u'status': u'status',
    u'date': u'data',
    u'dedication': u'dedykacja',
    u'copyright': u'prawa autorskie',
    u'abstract': u'streszczenie',
    u'attention': u'uwaga!',
    u'caution': u'ostrze\u017cenie!',
    u'danger': u'niebezpiecze\u0144stwo!',
    u'error': u'b\u0142\u0105d',
    u'hint': u'wzmianka',
    u'important': u'wa\u017cne',
    u'note': u'przypis',
    u'tip': u'wskaz\u00f3wka',
    u'warning': u'ostrze\u017cenie',
    u'contents': u'spis tre\u015bci'}
"""Mapping of node class name to label text."""

bibliographic_fields = {
    u'autor': u'author',
    u'autorzy': u'authors',
    u'organizacja': u'organization',
    u'adres': u'address',
    u'kontakt': u'contact',
    u'wersja': u'version',
    u'rewizja': u'revision',
    u'status': u'status',
    u'data': u'date',
    u'prawa autorskie': u'copyright',
    u'dedykacja': u'dedication',
    u'streszczenie': u'abstract'}
"""Polish (lowcased) to canonical name mapping for bibliographic fields."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
