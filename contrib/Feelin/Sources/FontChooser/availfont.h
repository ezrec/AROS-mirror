#ifndef FEELIN_ADJUSTFONT_AVAILFONT_H
#define FEELIN_ADJUSTFONT_AVAILFONT_H

struct FeelinAvailSizeNode
{
   struct FeelinAvailSizeNode      *Next;
   struct FeelinAvailSizeNode      *Prev;
   ULONG                            Size;
};

struct FeelinAvailFontNode
{
   struct FeelinAvailFontNode      *Next;
   struct FeelinAvailFontNode      *Prev;
   STRPTR                           Name;
   FList                            SizeList;
};

struct FeelinAvailFontHeader
{
   FList                            FontList;
   APTR                             Pool;
}; 

void                            f_fontlist_add(struct FeelinAvailFontHeader *Header,struct TextAttr *ta);
struct FeelinAvailFontHeader *  f_fontlist_create(ULONG Flags);
void                            f_fontlist_delete(struct FeelinAvailFontHeader * Header);

#endif
