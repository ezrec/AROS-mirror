#include "Private.h"

enum    {

        FV_XML_ID_BAR = FV_XMLDOCUMENT_ID_DUMMY,
        FV_XML_ID_COL,
        FV_XML_ID_FIXED,
        FV_XML_ID_MAX,
        FV_XML_ID_MIN,
        FV_XML_ID_PADDING,
        FV_XML_ID_SPAN,
        FV_XML_ID_WEIGHT,
        FV_XML_ID_WIDTH

        };

STATIC FDOCID xml_list_ids[] =
{
    { "bar",            3, FV_XML_ID_BAR      },
    { "col",            3, FV_XML_ID_COL      },
    { "fixed",          5, FV_XML_ID_FIXED    },
    { "max",            3, FV_XML_ID_MAX      },
    { "min",            3, FV_XML_ID_MIN      },
    { "padding",        7, FV_XML_ID_PADDING  },
    { "span",           4, FV_XML_ID_SPAN     },
    { "weight",         6, FV_XML_ID_WEIGHT   },
    { "width",          5, FV_XML_ID_WIDTH    },

    F_ARRAY_END
};

///titlebar_dispose
void titlebar_dispose(FClass *Class,FObject Obj)
{
     struct LocalObjectData *LOD = F_LOD(Class,Obj);

     LOD -> ColumnCount = 0;

     if (LOD -> TitleBar)
     {
          F_Dispose(LOD -> TitleBar);

          LOD -> TitleBar = NULL;
          LOD -> Columns = NULL;
     }
}
//+
///titlebar_new

/* The columns titles are handle as any other Line.  This  function  parses
the  given  format  string  and  create  a FLine structure extanded to have
enough space to hold FColumn strucs. This extanded space is pointed by  the
'Columns' field of the LocalObjectData structure.

The function will also free previously allocated ressources before creating
new ones.

Return FALSE on failure. */

int32 titlebar_new(FClass *Class,FObject Obj,STRPTR Fmt)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FObject xml;

    titlebar_dispose(Class,Obj);

    if (Fmt == NULL)
    {
        F_Log(0,"Columns format is NULL"); return FALSE;
    }
    else if (*Fmt == '\0')
    {
        F_Log(0,"Columns format is EMPTY (0x%08lx)",Fmt); return FALSE;
    }

    xml = XMLDocumentObject,

        "Source",      Fmt,
        "SourceType",  "Memory",

    End;

    if (xml != NULL)
    {
        uint32 id_Resolve = F_DynamicFindID("FM_Document_Resolve");
        FXMLMarkup *markup;
        uint32 n=0;

        F_Do(xml,(uint32) "FM_Document_AddIDs",xml_list_ids);

        for (markup = (FXMLMarkup *) F_Get(xml,(uint32) "Markups") ; markup ; markup = markup -> Next)
        {
            if (markup -> Name -> ID == FV_XML_ID_COL)
            {
                FXMLAttribute *attribute;
                uint32 span=1;

                for (attribute = (FXMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
                {
                    if (attribute -> Name -> ID == FV_XML_ID_SPAN)
                    {
                        span = F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                    }
                }

                n += span;
            }
        }

        if (n)
        {
            LOD -> TitleBar = F_NewP(LOD -> LinePool,
               sizeof (FLine) +                            // Struct header
               ((n + 1) * sizeof (STRPTR)) +               // Array of Strings
               ((n + 1) * sizeof (STRPTR)) +               // Array of PreParses
               ((n + 1) * sizeof (UWORD))  +               // Array of Widths
               (n * sizeof (struct FeelinList_Column)));   // Array of FeelinList_Columns

            if (LOD -> TitleBar)
            {
                LOD -> TitleBar -> Strings   = (STRPTR *)(((uint32)(LOD -> TitleBar) + sizeof (FLine)));
                LOD -> TitleBar -> PreParses = (STRPTR *)((uint32)(LOD -> TitleBar -> Strings)   + sizeof (STRPTR) * (n + 1));
                LOD -> TitleBar -> Widths    = (UWORD  *)((uint32)(LOD -> TitleBar -> PreParses) + sizeof (STRPTR) * (n + 1));
                LOD -> Columns               = (APTR)((uint32)(LOD -> TitleBar -> Widths) + sizeof (UWORD) * (n + 1));

                LOD -> TitleBar -> Position  = -1;

                LOD -> ColumnCount = n;

                for (n = 0, markup = (FXMLMarkup *) F_Get(xml,(uint32) "Markups") ; markup ; markup = markup -> Next, n++)
                {
                    if (markup -> Name -> ID == FV_XML_ID_COL)
                    {
                        FXMLAttribute *attribute;
                        uint32 span=0;

                        /*** default values ***/

                        LOD -> Columns[n].Padding = 4;

                        /*** reading ***/

                        for (attribute = (FXMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
                        {
                            switch (attribute -> Name -> ID)
                            {
                                case FV_XML_ID_BAR:
                                {
                                    if (F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_BOOLEAN,NULL,NULL))
                                    {
                                       LOD -> Columns[n].Flags |= FF_COLUMN_BAR;
                                    }
                                }
                                break;

                                case FV_XML_ID_FIXED:
                                {
                                    if (F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_BOOLEAN,NULL,NULL))
                                    {
                                       LOD -> Columns[n].Flags |= FF_COLUMN_FIXED;
                                    }
                                }
                                break;

                                case FV_XML_ID_PADDING:
                                {
                                    LOD -> Columns[n].Padding = F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                                }
                                break;

                                case FV_XML_ID_SPAN:
                                {
                                    if ((span = F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL)))
                                    {
                                       span--;
                                    }
                                }
                                break;

                                case FV_XML_ID_WEIGHT:
                                {
                                    LOD -> Columns[n].Weight = F_Do(xml,id_Resolve,attribute -> Data,FV_TYPE_INTEGER,NULL,NULL);
                                }
                                break;
                            }
                        }

                        if (span)
                        {
                            FColumn *ref = &LOD -> Columns[n];

                            while (span--)
                            {
                                n++;

                                LOD -> Columns[n].Flags    = ref -> Flags;
                                LOD -> Columns[n].Padding  = ref -> Flags;
                                LOD -> Columns[n].Weight   = ref -> Weight;
                            }
                        }
                    }
                }
            }
        }
        F_DisposeObj(xml);
    }
    return (LOD -> TitleBar) ? TRUE : FALSE;
}
//+

///titlebar_compute_dimensions
void titlebar_compute_dimensions(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    FLine *line = LOD -> TitleBar;

    LOD -> TitleBarH = 0;

    if (line)
    {
        if (_render)
        {
            FFramePublic *fd = LOD -> TitleFramePublic;
            uint32 j;
            BOOL prev_bar=FALSE;

            line_display(Class,Obj,line);

            line -> Height = 0;

            for (j = 0 ; j < LOD -> ColumnCount ; j++)
            {
                uint32 tw=0,th=0;

                if (line -> Strings[j])
                {
                    F_Do(LOD -> TDObj,FM_Set,

                        FA_TextDisplay_Font,       _font,
                        FA_TextDisplay_Width,      -1,
                        FA_TextDisplay_Height,     -1,
                        FA_TextDisplay_Contents,   line -> Strings[j],
                        FA_TextDisplay_PreParse,   line -> PreParses[j],

                        TAG_DONE);

                    F_Do(LOD -> TDObj,FM_Get,

                        FA_TextDisplay_Width,      &tw,
                        FA_TextDisplay_Height,     &th,

                        TAG_DONE);

                    /* frame borders ********************************************/

                    if (fd)
                    {
                        tw += MAX(fd -> Border[0].l,fd -> Border[1].l) + MAX(fd -> Border[0].r,fd -> Border[1].r) + MAX(fd -> Padding[0].l,fd -> Padding[1].l) + MAX(fd -> Padding[0].r,fd -> Padding[1].r);
                        th += MAX(fd -> Border[0].t,fd -> Border[1].t) + MAX(fd -> Border[0].b,fd -> Border[1].b) + MAX(fd -> Padding[0].t,fd -> Padding[1].t) + MAX(fd -> Padding[0].b,fd -> Padding[1].b);
                    }

                    /* padding flags ********************************************/

                    if (LOD -> Columns[j].Padding)
                    {
                        if (j == 0)
                        {
                            if (j + 1 < LOD -> ColumnCount)
                            {
                                LOD -> Columns[j].Flags |= FF_COLUMN_PADRIGHT;
                            }
                        }
                        else if (j + 1 < LOD -> ColumnCount)
                        {
                            LOD -> Columns[j].Flags |= FF_COLUMN_PADLEFT | FF_COLUMN_PADRIGHT;
                        }
                        else
                        {
                            LOD -> Columns[j].Flags |= FF_COLUMN_PADLEFT;
                        }
                    }

                    if (FF_COLUMN_PADLEFT & LOD -> Columns[j].Flags)
                    {
                        tw += LOD -> Columns[j].Padding;
                    }
                    if (FF_COLUMN_PADRIGHT & LOD -> Columns[j].Flags)
                    {
                        tw += LOD -> Columns[j].Padding;
                    }

                    /* bar flags ************************************************/

                    if (prev_bar)
                    {
                        LOD -> Columns[j].Flags |= FF_COLUMN_PREVBAR;
                        tw++;
                    }
                    else
                    {
                        LOD -> Columns[j].Flags &= ~FF_COLUMN_PREVBAR;
                    }

                    if ((FF_COLUMN_BAR & LOD -> Columns[j].Flags) && (j + 1 < LOD -> ColumnCount))
                    {
                        tw ++; prev_bar = TRUE;
                    }
                    else
                    {
                        prev_bar = FALSE;
                    }
                }

                line -> Height = MAX(line -> Height,th);
                line -> Widths[j] = tw;

                LOD -> Columns[j].Width = line -> Widths[j];
            }

            LOD -> TitleBarH = line -> Height;
        }
    }
}
//+
///titlebar_adjust_dimensions
void titlebar_adjust_dimensions(FClass *Class,FObject Obj)
{
   struct LocalObjectData *LOD = F_LOD(Class,Obj);
   FLine *line = LOD -> TitleBar;

   if (line)
   {
      uint32 total_width=0;
      uint32 i;

      for (i = 0 ; i < LOD -> ColumnCount ; i++)
      {
//         F_Log(0,"adjust >> col (%ld) weight (%ld) width (%ld)",i,LOD -> Columns[i].Weight,LOD -> Columns[i].Width);
         total_width += (LOD -> Columns[i].Weight) ? LOD -> Columns[i].Weight : LOD -> Columns[i].Width;
      }

      if (total_width)
      {
         uint32 space = _iw;

         for (i = 0 ; i < LOD -> ColumnCount ; i++)
         {
            if (LOD -> Columns[i].Flags & FF_COLUMN_FIXED)
            {
               LOD -> Columns[i].AdjustedWidth = LOD -> Columns[i].Width;
               space -= LOD -> Columns[i].Width;
               total_width -= (LOD -> Columns[i].Weight) ? LOD -> Columns[i].Weight : LOD -> Columns[i].Width;
            }
         }

         for (i = 0 ; i < LOD -> ColumnCount ; i++)
         {
            if (!(LOD -> Columns[i].Flags & FF_COLUMN_FIXED))
            {
               if (LOD -> Columns[i].Weight)
               {
                  LOD -> Columns[i].AdjustedWidth = space * LOD -> Columns[i].Weight / total_width;
               }
               else
               {
                  LOD -> Columns[i].AdjustedWidth = space * LOD -> Columns[i].Width / total_width;
               }
            }
         }
      }
   }
}
//+
///titlebar_setup
int32 titlebar_setup(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> TitleBarH = 0;

    if (LOD -> TitleBar)
    {
        uint32 j;
        uint32 data;

        for (j = 0 ; j < LOD -> ColumnCount ; j++)
        {
            LOD -> TitleBar -> Widths[j] = 0;
        }

/*** scheme ****************************************************************/

        data = F_Do(_app,FM_Application_Resolve,LOD -> p_title_scheme,NULL);

        if (data)
        {
            LOD -> TitleScheme = (FPalette *) F_Do(_display,FM_CreateColorScheme,data,LOD -> Scheme);
        }

        _render -> Palette = (LOD -> TitleScheme) ? LOD -> TitleScheme : LOD -> Scheme;

/*** frame *****************************************************************/

        LOD -> TitleFrame = FrameObject,

            FA_Frame,   LOD -> p_title_frame,
            FA_Back,    LOD -> p_title_back,

            End;

        if (LOD -> TitleFrame)
        {
            if (F_Do(LOD -> TitleFrame,FM_Frame_Setup,_render))
            {
                LOD -> TitleFramePublic = (FFramePublic *) F_Get(LOD -> TitleFrame,FA_Frame_PublicData);

                if (LOD -> TitleFramePublic -> Back == NULL)
                {
                    LOD -> Flags |= FF_LIST_TITLE_INHERITED_BACK;
                    LOD -> TitleFramePublic -> Back = (FObject) F_Get(Obj,FA_Back);
                }
            }
            else
            {
                F_Dispose(LOD -> TitleFrame);

                LOD -> TitleFrame = NULL;
            }
        }

        if (LOD -> TitleFrame == NULL)
        {
            return FALSE;
        }

        titlebar_compute_dimensions(Class,Obj);
    }
    return TRUE;
}
//+
///titlebar_cleanup
void titlebar_cleanup(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (_render)
    {
        if (LOD -> TitleFrame)
        {
            if (FF_LIST_TITLE_INHERITED_BACK & LOD -> Flags)
            {
                LOD -> TitleFramePublic -> Back = NULL;
                LOD -> Flags &= FF_LIST_TITLE_INHERITED_BACK;
            }

            F_Do(LOD -> TitleFrame,FM_Frame_Cleanup,_render);
            F_DisposeObj(LOD -> TitleFrame);

            LOD -> TitleFrame = NULL;
            LOD -> TitleFramePublic = NULL;
        }

        if (LOD -> TitleScheme)
        {
            F_Do(_display,FM_RemPalette,LOD -> TitleScheme);

            LOD -> TitleScheme = NULL;
        }
    }
}
//+

///titlebar_draw
void titlebar_draw(FClass *Class,FObject Obj)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> TitleBar)
    {
        FBox b;
        uint32 j;
        FPalette *previous_palette = _render -> Palette;

        line_display(Class,Obj,LOD -> TitleBar);

        b.x = _ix;
        b.y = _iy;
        b.h = LOD -> TitleBarH;

        if (LOD -> TitleFramePublic && ((FF_LIST_TITLE_INHERITED_BACK & LOD -> Flags) == 0))
        {
            F_Set(LOD -> TitleFramePublic -> Back,FA_ImageDisplay_Origin,(uint32) &b);
        }

        _render -> Palette = (LOD -> TitleScheme) ? LOD -> TitleScheme : LOD -> Scheme;

        for (j = 0 ; j < LOD -> ColumnCount; j++)
        {
            FRect r;
            b.w = LOD -> Columns[j].AdjustedWidth;

            if (LOD -> TitleFrame)
            {
                F_Do(LOD -> TitleFrame,FM_Frame_Draw,_render,&b,FF_Frame_Draw_Erase);

                r.x1 = b.x + LOD -> TitleFramePublic -> Border[0].l + LOD -> TitleFramePublic -> Padding[0].l;
                r.y1 = b.y + LOD -> TitleFramePublic -> Border[0].t + LOD -> TitleFramePublic -> Padding[0].t;
                r.x2 = b.x + b.w - 1 - LOD -> TitleFramePublic -> Border[0].r - LOD -> TitleFramePublic -> Padding[0].r;
                r.y2 = b.y + b.h - 1 - LOD -> TitleFramePublic -> Border[0].b - LOD -> TitleFramePublic -> Padding[0].b;
            }
            else
            {
                r.x1 = b.x;
                r.y1 = b.y;
                r.x2 = b.x + b.w - 1;
                r.y2 = b.y + b.h - 1;
            }

            if (FF_COLUMN_PADLEFT & LOD -> Columns[j].Flags)
            {
                r.x1 += LOD -> Columns[j].Padding;
            }
            if (FF_COLUMN_PADRIGHT & LOD -> Columns[j].Flags)
            {
                r.x2 -= LOD -> Columns[j].Padding;
            }

            if (LOD -> TitleBar -> Strings[j])
            {
                F_Do(LOD -> TDObj,FM_Set,

                    FA_TextDisplay_Width,      r.x2 - r.x1 + 1,
                    FA_TextDisplay_Height,     b.h,
                    FA_TextDisplay_Contents,   LOD -> TitleBar -> Strings[j],
                    FA_TextDisplay_PreParse,   LOD -> TitleBar -> PreParses[j],

                    TAG_DONE);

                F_Do(LOD -> TDObj,FM_TextDisplay_Draw,&r);
            }

            b.x += LOD -> Columns[j].AdjustedWidth;
        }
        _render -> Palette = previous_palette;
    }
}
//+
