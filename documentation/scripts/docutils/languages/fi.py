# Author: David Goodger
# Contact: goodger@users.sourceforge.net
# Revision: $Revision: 16716 $
# Date: $Date: 2003-03-10 13:53:09 +0100 (Mon, 10 Mar 2003) $
# Copyright: This module has been placed in the public domain.

"""
Finnish-language mappings for language-dependent features of Docutils.
"""

__docformat__ = 'reStructuredText'


from docutils import nodes


labels = {
      'author':		u'Tekij\u00e4',
      'authors':	u'Tekij\u00e4t',
      'organization':	'Organisaatio',
      'address':	'Osoite',
      'contact':	'Yhteyden otto',
      'version':	'Versio',
      'revision':	'Tarkistettu laitos',
      'status':		'Tila',
      'date':		u'P\u00e4iv\u00e4ys',
      'copyright':	'Copyright',
      'dedication':	'Omistus',
      'abstract':	u'Tiivistelm\u00e4',
      'attention':	'Huomio!',
      'caution':	'Varoitus!',
      'danger':		'Vaara!',
      'error':		'Virhe',
      'hint':		'Vinkki',
      'important':	u'T\u00e4rke\u00e4\u00e4',
      'note':		'Pane merkille',
      'tip':		'Vinkki',
      'warning':	'Varoitus',
      'contents':	u'Sis\u00e4lt\u00f6'}
"""Mapping of node class name to label text."""

bibliographic_fields = {
      'author': nodes.author,
      'authors': nodes.authors,
      'organization': nodes.organization,
      'address': nodes.address,
      'contact': nodes.contact,
      'version': nodes.version,
      'revision': nodes.revision,
      'status': nodes.status,
      'date': nodes.date,
      'copyright': nodes.copyright,
      'dedication': nodes.topic,
      'abstract': nodes.topic}
"""Field name (lowcased) to node class name mapping for bibliographic fields
(field_list)."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
