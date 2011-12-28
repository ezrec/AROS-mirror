#include "includes.h"
#include "HTML.h"
#include "mosaic.h"
#include "globals.h"

#include <proto/diskfont.h>

extern AppData Rdata;


void setup_font(struct TextAttr *t, char *name, char *pref)
{
  char *p;
  int style = 0;
  int size = 0;

  p = strchr(pref, '/');
  if (!p) {
    printf("Malformed fontpref: %s\n", pref);
    exit(1);
  }
  ++p;
  stccpy(name, pref, (p - pref));
  strcat(name, ".font");

  size = atoi(p);

  if (p = strchr(p, '/')) {	/* look for style */
    p++;
    if (strnicmp(p, "bold", 4) == 0) style |= FSF_BOLD;
    if (strnicmp(p, "italic", 6) == 0) style |= FSF_ITALIC;
  }

  t->ta_Name = name;
  t->ta_YSize = size;
  t->ta_Style = style;
  t->ta_Flags = 0;
}


void load_font(struct TextFont **f, struct TextAttr *a)
{
  struct TextFont *temp;

  *f = OpenDiskFont(a);
  if (!*f) {
    printf("ERROR: no font %s/%d\n", a->ta_Name, a->ta_YSize);
    // WE SHOULD FREE LOTS OF STUFF FIRST!  DUH!
    exit(1);
  }

  if ((*f)->tf_Style != a->ta_Style) {
    temp = *f;
    *f = malloc(sizeof(struct TextFont));
    **f = *temp;
    (*f)->tf_Style = a->ta_Style;
  }
}


/*------------------------------------------------------------------------
------------------------------------------------------------------------*/
void open_fonts(void)
{
  static char name[15][50];

  setup_font(&ta_norm, name[0], Rdata.font);
  setup_font(&ta_fixed, name[1], Rdata.fixedfont);
  setup_font(&ta_h1, name[2], Rdata.h1font);
  setup_font(&ta_h2, name[3], Rdata.h2font);
  setup_font(&ta_h3, name[4], Rdata.h3font);
  setup_font(&ta_h4, name[5], Rdata.h4font);
  setup_font(&ta_h5, name[6], Rdata.h5font);
  setup_font(&ta_h6, name[7], Rdata.h6font);
  setup_font(&ta_bold, name[8], Rdata.boldfont);
  setup_font(&ta_ital, name[9], Rdata.italicfont);
  setup_font(&ta_listing, name[10], Rdata.listingfont);
  setup_font(&ta_plain, name[11], Rdata.plainfont);
  setup_font(&ta_address, name[12], Rdata.addressfont);

  load_font(&tf_norm, &ta_norm);
  load_font(&tf_bold, &ta_bold);
  load_font(&tf_ital, &ta_ital);
  load_font(&tf_fixed, &ta_fixed);
  load_font(&tf_h1, &ta_h1);
  load_font(&tf_h2, &ta_h2);
  load_font(&tf_h3, &ta_h3);
  load_font(&tf_h4, &ta_h4);
  load_font(&tf_h5, &ta_h5);
  load_font(&tf_h6, &ta_h6);
  load_font(&tf_listing, &ta_listing);
  load_font(&tf_plain, &ta_plain);
  load_font(&tf_address, &ta_address);
}

#define close_font(font) if(font) CloseFont(font);

void close_fonts(void)
{
  close_font(tf_norm);
  close_font(tf_bold);
  close_font(tf_ital);
  close_font(tf_fixed);
  close_font(tf_h1);
  close_font(tf_h2);
  close_font(tf_h3);
  close_font(tf_h4);
  close_font(tf_h5);
  close_font(tf_h6);
  close_font(tf_listing);
  close_font(tf_plain);
  close_font(tf_address);
}
