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
Greek-language mappings for language-dependent features of Docutils.
"""

__docformat__ = 'reStructuredText'

labels = {
      'author': 'Συγγραφέας',
      'authors': 'Συγγραφείς',
      'organization': 'Οργανισμός',
      'address': 'Διεύθυνση',
      'contact': 'Επικοινωνία',
      'version': 'Έκδοση',
      'revision': 'Αναθεώρηση',
      'status': 'Στάτους',
      'date': 'Ημερομηνία',
      'copyright': 'Copyright',
      'dedication': 'Αφιέρωση',
      'abstract': 'Περίληψη',
      'attention': 'Προσοχή!',
      'caution': 'Προσοχή!',
      'danger': '!ΚΙΝΔΥΝΟΣ!',
      'error': 'Σφάλμα',
      'hint': 'Σημείωση',
      'important': 'Συμαντικό',
      'note': 'Σημείωση',
      'tip': 'Συμβουλή',
      'warning': 'Προειδοποίηση',
      'contents': 'Περιεχόμενα'}
"""Mapping of node class name to label text."""

bibliographic_fields = { 
      'συγγραφέας': 'author',
      'συγγραφείς': 'authors',
      'οργανισμός': 'organization',
      'διεύθυνση': 'address',
      'επικοινωνία': 'contact',
      'έκδοση': 'version',
      'αναθεώρηση': 'revision',
      'στάτους': 'status',
      'ημερομηνία': 'date',
      'copyright': 'copyright',
      'αφιέρωση': 'dedication',
      'περίληψη': 'abstract'}
"""Greek (lowcased) to canonical name mapping for bibliographic fields."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
