'''Convert faq.FAQ object into HTML.'''

import faq, os.path, xml2html
from util import Page, arosRC

def gen (rcfile, linkBoxItem, *dirs):
    faqobj = apply (faq.FAQ, dirs)
    xml = faqobj.toXml ()

    page = Page (arosRC, linkBoxItem)
    xml2html.elementToHtml (xml, page)
    return page

