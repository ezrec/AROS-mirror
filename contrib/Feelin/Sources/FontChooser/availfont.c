#include "Private.h"
#include "availfont.h"

///f_fontlist_add
void f_fontlist_add(struct FeelinAvailFontHeader *Header,struct TextAttr *ta)
{
    if (ta -> ta_Name && ta -> ta_YSize)
    {
        struct FeelinAvailFontNode *font_node;
        struct FeelinAvailSizeNode *size_node;

        for (font_node = (struct FeelinAvailFontNode *) Header -> FontList.Head ; font_node ; font_node = font_node -> Next)
        {
            if (Stricmp(ta -> ta_Name,font_node -> Name) == 0)
            {
                for (size_node = (struct FeelinAvailSizeNode *)(font_node -> SizeList.Head) ; size_node ; size_node = size_node -> Next)
                {
                    if (ta -> ta_YSize == size_node -> Size)
                    {
                        return;
                    }
                }

                if (!size_node)
                {
                    if ((size_node = F_NewP(Header -> Pool,sizeof (struct FeelinAvailSizeNode))) != NULL)
                    {
                        struct FeelinAvailSizeNode *size_prev;

                        size_node -> Size = ta -> ta_YSize;

                        for (size_prev = (struct FeelinAvailSizeNode *)(font_node -> SizeList.Tail) ; size_prev ; size_prev = size_prev -> Prev)
                        {
                            if (size_node -> Size > size_prev -> Size) break;
                        }

                        F_LinkInsert(&font_node -> SizeList,(FNode *) size_node,(FNode *) size_prev);
                    }

                    return;
                }
            }
        }

        if (!font_node)
        {
            if ((font_node = F_NewP(Header -> Pool,sizeof (struct FeelinAvailFontNode))) != NULL)
            {
                ULONG len = F_StrLen(ta -> ta_Name);

                if ((font_node -> Name = F_NewP(Header -> Pool,len + 1)) != NULL)
                {
                    CopyMem(ta -> ta_Name,font_node -> Name,len);

                    if ((size_node = F_NewP(Header -> Pool,sizeof (struct FeelinAvailSizeNode))) != NULL)
                    {
                        struct FeelinAvailFontNode *font_prev;

                        size_node -> Size = ta -> ta_YSize;

                        F_LinkTail(&font_node -> SizeList,(FNode *) size_node);

                        for (font_prev = (struct FeelinAvailFontNode *)(Header -> FontList.Tail) ; font_prev ; font_prev = font_prev -> Prev)
                        {
                            if (Stricmp(font_node -> Name,font_prev -> Name) > 0) break;
                        }

                        F_LinkInsert(&Header -> FontList,(FNode *)font_node,(FNode *)font_prev);

//                  F_Log(0,"af 0x%08lx - Name %s - Size %ld (0x%08lx)",af,af -> Name,Size,af -> Sizes);

                        return;
                    }
                    F_Dispose(font_node -> Name);
                }
                F_Dispose(font_node);
            }
        }
    }
}
//+
///f_fontlist_create
struct FeelinAvailFontHeader * f_fontlist_create(bits32 Flags)
{
    APTR pool = F_CreatePool(1024,FA_Pool_Items,1,TAG_DONE);

    if (pool)
    {
        struct FeelinAvailFontHeader *header = F_NewP(pool,sizeof (struct FeelinAvailFontHeader));

        if (header)
        {
            /* create AvailFont buffer */

            struct AvailFontsHeader *afh;
            UWORD af_size = 1024;
            UWORD af_shortage;

            header -> Pool = pool;

            do
            {
                afh = (struct AvailFontsHeader *) F_NewP(pool,af_size);

                if (afh)
                {
                    af_shortage = AvailFonts((STRPTR)(afh), af_size, AFF_MEMORY | AFF_DISK | AFF_SCALED);

                    if (af_shortage)
                    {
                        F_Dispose(afh);
                        af_size += af_shortage;
                    }
                }
                else
                {
                    F_Log(FV_LOG_USER,"Unable to allocate AvailFonts buffer");

                    break;
                }
            }
            while (af_shortage);

            if (afh)
            {
                struct FeelinAvailFontNode *font_node;
                ULONG i;
                struct AvailFonts *af;

                af = (struct AvailFonts *)((ULONG)(afh) + (sizeof (struct AvailFontsHeader)));

                for (i = 0 ; i < afh -> afh_NumEntries ; i++)
                {
                    if (AFF_DISK & af -> af_Type)
                    {
                        f_fontlist_add(header,&af -> af_Attr);
                    }

                    af++;
                }

                af = (struct AvailFonts *)((ULONG)(afh) + (sizeof (struct AvailFontsHeader)));

                for (i = 0 ; i < afh -> afh_NumEntries ; i++)
                {
                    if (AFF_MEMORY & af -> af_Type)
                    {
                        f_fontlist_add(header,&af -> af_Attr);
                    }

                    af++;
                }

                /* remove '.font' be replacing the '.' with a '\0' */

                for (font_node = (struct FeelinAvailFontNode *)(header -> FontList.Head) ; font_node ; font_node = font_node -> Next)
                {
                    STRPTR str = font_node -> Name;

                    if (str)
                    {
                        while (*str && *str != '.') str++;

                        if (*str == '.') *str = 0;
                    }
                }

                F_Dispose(afh);
            }
            return header;
        }
        F_DeletePool(pool);
    }
    return NULL;
}
//+
///f_fontlist_delete
void f_fontlist_delete(struct FeelinAvailFontHeader * Header)
{
    if (Header)
    {
        F_DeletePool(Header -> Pool);
    }
}
//+
