#ifndef _LIBINTL_H
#define _LIBINTL_H 1

char *gettext(const char * msgid);

char *ngettext(const char *msgid1, const char *msgid2, unsigned long int n);

char *textdomain(const char *domainname);

char *bindtextdomain(const char *domainname,const char *dirname);

char *bind_textdomain_codeset(const char *domainname, const char *codeset);

char *dgettext(const char *domainname, const char *msgid);

char *dcgettext(const char *domainname,const char *msgid, int category);

char *dngettext(const char *domainname, const char *msgid1,const char *msgid2, unsigned long int n);

char *dcngettext(const char *domainname, const char *msgid1,const char *msgid2, unsigned long int n, int category);

#endif
