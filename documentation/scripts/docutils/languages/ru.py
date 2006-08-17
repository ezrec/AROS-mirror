# Author: David Goodger; Sergey Mineychev
# Contact: goodger@users.sourceforge.net; keen@aros.org
# Revision: $Revision: 16716 $
# Date: $Date: 2006-08-12 16:53:09 +0400 (а†аН, 10 аМаАб• 2003) $
# Copyright: This module has been placed in the public domain.

"""
Russian-language mappings for language-dependent features of Docutils.
"""

__docformat__ = 'reStructuredText'


from docutils import nodes


labels = {
      'author': 'Автор',
      'authors': 'Авторы',
      'organization': 'Организация',
      'address': 'Адрес',
      'contact': 'Контакт',
      'version': 'Версия',
      'revision': 'Ревизия',
      'status': 'Статус',
      'date': 'Дата',
      'copyright': 'Copyright',
      'dedication': 'Назначение',
      'abstract': 'Обзор',
      'attention': 'Внимание!',
      'caution': 'Острожно!',
      'danger': '!ОПАСНО!',
      'error': 'Ошибка',
      'hint': 'Совет',
      'important': 'Важно',
      'note': 'Заметка',
      'tip': 'Подсказка',
      'warning': 'Предупреждение',
      'contents': 'Содержание'}
"""Mapping of node class name to label text."""

bibliographic_fields = {
      'Автор': nodes.author,
      'Авторы': nodes.authors,
      'Организация': nodes.organization,
      'Адрес': nodes.address,
      'Контакт': nodes.contact,
      'Версия': nodes.version,
      'Ревизия': nodes.revision,
      'Статус': nodes.status,
      'Дата': nodes.date,
      'copyright': nodes.copyright,
      'Назначение': nodes.topic,
      'Обзор': nodes.topic}
"""Field name (lowcased) to node class name mapping for bibliographic fields
(field_list)."""

author_separators = [';', ',']
"""List of separator strings for the 'Authors' bibliographic field. Tried in
order."""
