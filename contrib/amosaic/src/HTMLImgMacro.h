/* HTMLImgMacro.h  mjw 21 Jan 1995 - only included in HTMLamiga.c 
   this is used for images */

// was hw->html->rp->BitMap->Depth
#define WIDG_DEPTH TO_HTMLGetScreenDepth() 
#define MAKE_IMGINFO_FROM_BITMAP(name) \
  if (!name) \
    { \
      name = (ImageInfo *)malloc (sizeof (ImageInfo)); \
      name->mask_bitmap=NULL; \
      name->ismap = 0; \
      name->width = name##_width; \
      name->height = name##_height; \
      name->image_data = NULL; \
      /* Bandaid for bug in Eric's code. */ \
      name->internal = 1; \
      name->image = \
        CreateBitMapFromXBMData \
          (name##_bits,  \
           name##_width, name##_height, \
           WIDG_DEPTH); \
    } \
  if (!name->image) \
    { \
      name->image = \
        CreateBitMapFromXBMData \
          (name##_bits,  \
           name##_width, name##_height, \
           WIDG_DEPTH); \
    }

#define RETURN_IMGINFO_FROM_BITMAP(x) \
{ \
  MAKE_IMGINFO_FROM_BITMAP(x); \
  return x; \
}

