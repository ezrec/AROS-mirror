#ifndef __G_STRFUNCS_H__
#define __G_STRFUNCS_H__

gchar* g_strdup (const gchar *str);
gpointer g_memdup (gconstpointer mem, guint         byte_size);
gchar* g_strndup (const gchar *str, guint        n);
gchar* g_strnfill (guint length, gchar fill_char);
gchar* g_strdup_vprintf (const gchar *format, va_list      args1);
gchar* g_strdup_printf (const gchar *format, ...) ;
gchar* g_strconcat (const gchar *string1, ...);
gdouble g_strtod (const gchar *nptr, gchar **endptr);
gchar* g_strerror (gint errnum);
gchar* g_strsignal (gint signum);
guint g_printf_string_upper_bound (const gchar* format, va_list      args);
void g_strdown (gchar *string);
void g_strup (gchar *string);
void g_strreverse (gchar *string);
gint g_strcasecmp (const gchar *s1, const gchar *s2);
gint g_strncasecmp (const gchar *s1, const gchar *s2, guint n);
gchar* g_strdelimit (gchar   *string, const gchar *delimiters, gchar    new_delim);
gchar* g_strescape (gchar *string);
gchar* g_strchug (gchar *string);
gchar* g_strchomp (gchar *string);
gchar**
g_strsplit (const gchar *string, const gchar *delimiter, gint         max_tokens);
void g_strfreev (gchar **str_array);
gchar* g_strjoinv (const gchar  *separator, gchar       **str_array);
gchar* g_strjoin (const gchar  *separator, ...);

#endif /* __G_STRFUNCS_H__ */
