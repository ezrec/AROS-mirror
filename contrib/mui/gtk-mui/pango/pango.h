#ifndef __PANGO_FONT_H__
#define __PANGO_FONT_H__

#include <gtk/gtk.h>

typedef struct _PangoFontDescription PangoFontDescription;

/* o1i: the following ins from pango/fonts.c,
 * well, for me it's more logical to place
 * it in a header.. */

struct _PangoFontDescription {
  char *family_name;

#if 0
  PangoStyle style;
  PangoVariant variant;
  PangoWeight weight;
  PangoStretch stretch;
#endif

  guint16 mask;
  guint static_family : 1;
  guint size_is_absolute : 1;

  int size;
};

#endif /* __PANGO_FONT_H__ */
